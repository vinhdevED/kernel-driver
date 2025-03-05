#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h> 	// For class_create and device_create


#define LED_0		500	
#define LED_1		501	
#define LED_2		502	
#define LED_3		503	
#define LED_4		504	
#define LED_5		505	
#define LED_6		506

#define NUM_LEDS	7
#define MINOR_FIRST	0
#define MINOR_COUNT	1
#define DEV_NAME	"led_matrix_zcu102"
#define CLASS_NAME	"led_matrix_class"

static dev_t dev_num;
static struct cdev led_cdev;
static struct class *led_class; 

static int led_global_gpios[NUM_LEDS] = {LED_0, LED_1, LED_2, LED_3, LED_4, LED_5, LED_6};
static int led_gpio_state[NUM_LEDS] = {0, 0, 0, 0, 0, 0, 0};

// Protoype cua ham
static int led_open(struct inode *, struct file *);
static int led_release(struct inode *, struct file *);
static ssize_t led_write(struct file *, const char __user *, size_t, loff_t *);
static ssize_t led_read(struct file *, char __user *, size_t, loff_t *);

static int __init char_dev_init(void);
static void __exit char_dev_exit(void);


struct file_operations f_ops = {
	.owner	=	THIS_MODULE,
	.open	= 	led_open,
	.read	=	led_read,
	.write	=	led_write,
	.release=	led_release
};


/*
** This function will be called when we open the Device file
*/
static int led_open(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "LED GPIO released,  but keeping the last state\n");
        return 0;
}
/*
** This function will be called when we close the Device file
*/
static int led_release(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "LED GPIO released\n");
    	return 0;
}
/*
** This function will be called when we read the Device file
*/
static ssize_t led_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
        pr_info("Driver Read Function Called...!!!\n");
        return 0;
}
/*
** This function will be called when we write the Device file
*/
static ssize_t led_write(struct file *filp, const char __user *buf, size_t len, loff_t *off){

	char buffer[4]={0};
	int led_value, led_index;
	
	if( (len < 3) || (len > 4) ){
		printk(KERN_ALERT "Invalid input format. Use '1-0' or '0-2'\n");
		return -EINVAL;
	}
	
	// Copy buffer from user to kernel through this function
	if(copy_from_user(buffer, buf, len)){
		return -EFAULT;
	}
	
	buffer[len] = '\0';
	
	// Check and take input value
	/*
	RETURN --> SUCCESS : take successfully the numbers of argument
	*/
	if(sscanf(buffer, "%d-%d", &led_value,&led_index) != 2){
		printk(KERN_ALERT "Expected '1-0' or '0-2'\n");
		return -EINVAL;
	}
	
	// Check valid LED value
	if(led_value != 0 && led_value != 1){
		printk(KERN_ALERT "LED VALUE ERROR - Expected 0/1 !!!\n");
		return -EINVAL;
	}
	
	// Check valid LED index
	if( (led_index < 0) || (led_index >= NUM_LEDS) ){
		printk(KERN_ALERT "LED INDEX ERROR - Range from 0-6 !!!\n");
		return -EINVAL;
	}
	
	// Set value and keep state of specific LED
	gpio_set_value(led_global_gpios[led_index], led_value);
	led_gpio_state[led_index] = led_value;
	
	printk(KERN_INFO "LED %d turned %s\n",led_index,led_value ? "ON" : "OFF");
	
        return len;
}

static int __init char_dev_init(void){

	int ret,i;
	
	
	for(i = 0; i< NUM_LEDS;i++){
		// Check list gpios
		if(!gpio_is_valid(led_global_gpios[i])){
			printk(KERN_ALERT "[leds_gpio]: LED GPIO %d is not valid\n", led_global_gpios[i]);
			return -ENODEV;
		}
		
		//  Request GPIO
		ret = gpio_request(led_global_gpios[i], "led_gpio_rpi3");
		if(ret){
			printk(KERN_ALERT "[leds_gpio]: Failed to request GPIO %d\n", led_global_gpios[i]);
			return ret;
		}
		
		// Set direction "output"  for GPIO
		gpio_direction_output(led_global_gpios[i],0);
		printk(KERN_INFO " LED Driver : GPIO %d is initialized as output\n",  led_global_gpios[i]);
	}
	
	
	// Allocate device number
	ret = alloc_chrdev_region(&dev_num, MINOR_FIRST, MINOR_COUNT, DEV_NAME);
	if( ret < 0){
		printk(KERN_ALERT "[led_device]: Failed to allocate device number");
		return ret;	
	}
	
	// Initialize cdev structure
	cdev_init(&led_cdev, &f_ops);
	
	// Register cdev structure into Kernel
	ret  =  cdev_add(&led_cdev, dev_num, MINOR_COUNT); 
	if( ret < 0) {
		unregister_chrdev_region(dev_num, MINOR_COUNT);
		printk(KERN_ALERT "[led_device]: Failed to add cdev  \n");
		return ret;
	}
	
	// Create class for device in sysfs
	led_class =  class_create(THIS_MODULE, CLASS_NAME);
	if(IS_ERR(led_class)){
		cdev_del(&led_cdev);
		unregister_chrdev_region(dev_num, MINOR_COUNT);
		printk(KERN_ALERT "[led_device]: Failed  to create  class\n");
		return  PTR_ERR(led_class);
	}
	
	// Create device file in /dev
	if(device_create(led_class, NULL, dev_num, NULL,  DEV_NAME) == NULL ){
		class_destroy(led_class);
		cdev_del(&led_cdev);
		unregister_chrdev_region(dev_num, MINOR_COUNT);
		printk(KERN_ALERT "[led_device]: Failed to create device \n");
		return -1;
	}
	
	printk(KERN_INFO "[led_device]: Driver loaded successfully !!!\n");
	printk(KERN_INFO "[led_device]: Registered with major %d, minor %d\n", MAJOR(dev_num), MINOR(dev_num));
    	return 0;
}


static void __exit char_dev_exit(void){
	int i;
	device_destroy(led_class, dev_num);
	class_destroy(led_class);
	cdev_del(&led_cdev);
	unregister_chrdev_region(dev_num,MINOR_COUNT);
	printk(KERN_INFO "[led_device]: Unregistered the device \n");
	
	for(i = 0; i < NUM_LEDS; i++) {
        	gpio_free(led_global_gpios[i]);
        	printk(KERN_INFO "LED Driver: GPIO %d released\n", led_global_gpios[i]);
    	}
    	
	printk(KERN_INFO "led_device]: %d LEDs is controled by this driver unloaded !!!\n",NUM_LEDS);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR(" -V8- ");
MODULE_DESCRIPTION("A simple Control LED Driver for ZYNQMP-ZCU102");

module_init(char_dev_init);
module_exit(char_dev_exit);


