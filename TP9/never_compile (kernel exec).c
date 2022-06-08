/********************************
*
*   Device driver for an echo device (see lab assignment up to exercise 6)
*
*   Note that there are many possible solutions, this is just a possible one
*
*********************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("I'd rather not say");
MODULE_DESCRIPTION("Simple echo character device driver");
MODULE_VERSION("0:0.1");

/** device driver functions  **/
static int echo_open(struct inode *inode, struct file *filp);
static int echo_release(struct inode *inode, struct file *filp);
ssize_t echo_read(struct file *filep, char __user *buff, size_t count, loff_t *offp);
ssize_t echo_write(struct file *filep, const char __user *buff, size_t count, loff_t *offp);

/** module functions **/
static int echo_init(void);
static void echo_exit(void);

/**  globas variables for the device driver  **/
static dev_t mydev;
static struct cdev *mycdev;
static struct file_operations myfops = {
    .owner = THIS_MODULE,
	.read = echo_read,
	.write = echo_write,
	.open = echo_open,
	.release = echo_release,
	.llseek = no_llseek
};

// size of the buffer to hold the string to echo (see echo_write)
#define WR_BUF_SIZE 100
// total number of echoed chars since load --> initialize in echo_init, too
int total_echoed=0; 
// same in ascii format - a string
char str_total_echoed[10];

static int echo_open(struct inode *inode, struct file *filp)
{
    // TODO (Exercise 5) Properly link file's private_data
    filp->private_data = mycdev;

    // TODO (Exercise 6) make this a non-seekable device
	nonseekable_open(inode, filp);

	// just for debug:
	//  printk(KERN_INFO "echo_open(): Returning\n");
    // return without error
	return 0;
}

static int echo_release(struct inode *inode, struct file *filp)
{
	// just for debug:
	//  printk(KERN_INFO "echo_release(): Returning\n");
    // return without error
    return 0;
}

ssize_t echo_read(struct file *filep, char __user *buff, size_t count, loff_t *offp)
{
	int buf_size = 0;

	// if total number of echoed chars is zero, return error
	if( total_echoed == 0 )
		return -1;

	buf_size = strlen( str_total_echoed );  // number of chars in the number string
	str_total_echoed[buf_size]=0;  // make sure it is null terminated
	buf_size++;  // include termination ('0') in the size

	// protection for limited buffer size (buffer smaller than 'count' to read
	if( count < buf_size ){
		// trying to read just part of the string with the number -- just return error
		return -1;  // error copying
	}
	
	// read is reading enough bytes to read the whole string
	if( copy_to_user( (void __user *) buff, (const void *) str_total_echoed, (unsigned long) buf_size ) != 0)
		return -1;  // error copying

	// reset echoed chars counter (number and string)
	total_echoed = 0;
	str_total_echoed[0]=0;
	
	// just for debug:
	//	printk(KERN_INFO "echo_read(): Returning\n");
    // return number of bytes read (returned to the user)
    return buf_size;
}

ssize_t echo_write(struct file *filep, const char __user *buff, size_t count, loff_t *offp)
{
	// internal kernel buffer for the string to be echoed
	char write_buffer[WR_BUF_SIZE];
	
	// protection for limited buffer size (buffer smaller than string to echo
	if( count < WR_BUF_SIZE ){
		// copy 'count' bytes from user buffer to kernel buffer (write_buffer)
		if( copy_from_user((void *) write_buffer, (const void __user *) buff, (unsigned long) count) != 0)
			return -1;  // error copying
		write_buffer[count]=0;  // null terminated string

		total_echoed += count;  // accumulate number of characters echoed
		sprintf(str_total_echoed, "%d", total_echoed);  // convert to ascii (string)

		printk(KERN_INFO "%s", write_buffer);  // actual echo!
		
		return count;  // the whole 'count' was echoed
	}
	else{   // write just the size of the buffer each call
			// the remainder will be written by subsequent calls to write()
		// copy 'WR_BUF_SIZE-1' bytes from user buffer to kernel buffer (write_buffer)
		if( copy_from_user((void *) write_buffer, (const void __user *) buff, (unsigned long) (WR_BUF_SIZE-1)) != 0)
			return -1;  // error copying
		write_buffer[WR_BUF_SIZE-1]=0; // again, null terminate this fragment

		total_echoed += WR_BUF_SIZE-1; // accumulate number of characters echoed
		sprintf(str_total_echoed, "%d", total_echoed); // convert to ascii (string)

		printk(KERN_INFO "%s", write_buffer);  // actul echo!

	    return (WR_BUF_SIZE-1);  // just WR_BUF_SIZE-1 chars were echoed
	}

	// debug:
	//    printk(KERN_INFO "echo_write(): Returning\n");
	//    return count;
}


/** module functions  **/

static int echo_init(void)
{
	int result = -1;

	// TODO (Exercise 2) register device driver so that:
	// - Driver name is echo 
	// - Major number is dynamically assigned
	// - Minor number starts from 0
	// - Only one device needs to be managed by the driver
	result = alloc_chrdev_region(&mydev, 0, 1, "echo");

	if (result < 0){
		printk(KERN_ERR "Failed to register echo device driver\n");
		return result;
	}

    // TODO (Exercise 4) Create struture to represent char devices
    mycdev = cdev_alloc();
    mycdev->owner = THIS_MODULE;
    mycdev->ops = &myfops;  // connect my functions to the respective system calls

    // TODO (Exercise 4) Register character device into the kernel
    result = cdev_add(mycdev, mydev, 1);   // now it appears in /dev/...

    if (result < 0){
        printk(KERN_ERR "Failed to register character device info\n");
        return result;
    }

	// TODO (Exercise 2) print "Echo device driver registered with major number X"
	// to the kernel logging buffer so that:
	// - X is the obtained major number during registration
	// - Message printed using the informational log evel
	printk(KERN_INFO "Echo device driver registered with major number %d\n", MAJOR(mydev));

	// initialize the count of echoed characters
	total_echoed = 0;
	str_total_echoed[0] = 0;

	return result;
}

static void echo_exit(void)
{
    // TODO (Exercise 4) deregister character device

    // TODO (Exercise 2) deregister the device driver's device numbers
	unregister_chrdev_region(mydev, 1);

	// TODO (Exercise 2) print "Echo device driver deregistered" to the kernel
	// logginf buffer using the information log level
	printk(KERN_INFO "Echo device driver deregistered\n");
}

module_init(echo_init);
module_exit(echo_exit);