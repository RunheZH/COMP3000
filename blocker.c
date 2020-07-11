#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <asm/uaccess.h>

#define DEVICE_NAME "blocker"
#define CLASS_NAME  "myclass"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("R. Zhong, T. Vashistha");
MODULE_DESCRIPTION("Create /dev/blocker");

// automatically determined device number
static int    majorNumber;
// data from userspace
static char   message[256] = {0};
// its size
static short  size_of_message;

static struct class* driverClass = NULL;
// driver device struct pointer
static struct device* driverDevice = NULL;
// driver prototype functions

static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

/* Driver-operation associations
*/
static struct file_operations fops = {
  .open = dev_open,
  .read = dev_read,
  .write = dev_write,
  .release = dev_release,
};

/* Initialization function
*/
static int __init blocker_init(void) {
  printk(KERN_INFO "blocker: initializing\n");
  // dynamically allocate a major number
  majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
  if (majorNumber<0) {
    printk(KERN_ALERT "blocker: failed to allocate major number\n");
    return majorNumber;
  }
  printk(KERN_INFO "blocker: registered with major number %d\n", majorNumber);
  // register device class
  driverClass = class_create(THIS_MODULE, CLASS_NAME);
  if (IS_ERR(driverClass)) {
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_ALERT "blocker: failed to register device class\n");
    return PTR_ERR(driverClass);
  }
  printk(KERN_INFO "blocker: device class registered\n");
  // register device driver
  driverDevice = device_create(driverClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
  if (IS_ERR(driverDevice)) {
  // if error, clean up
    class_destroy(driverClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_ALERT "blocker: failed to create device\n");
    return PTR_ERR(driverDevice);
  }
  printk(KERN_INFO "blocker: device class created\n");
  return 0;
}

static void __exit blocker_exit(void) {
  // remove device
  device_destroy(driverClass, MKDEV(majorNumber, 0));
  // unregister device class
  class_unregister(driverClass);
  // remove device class
  class_destroy(driverClass);
  // unregister major number
  unregister_chrdev(majorNumber, DEVICE_NAME);
  printk(KERN_INFO "blocker: closed\n");
}

/* Called each time the device is opened.
* inodep = pointer to inode
* filep = pointer to file object
*/
static int dev_open(struct inode *inodep, struct file *filep) {
   /* struct file* filp = NULL;
    mm_segment_t oldfs;
    int err = 0;

    oldfs = get_fs();
    set_fs(get_ds());
    filp = filp_open(path, flags, rights);
    set_fs(oldfs);
    if(IS_ERR(filp)) {
        err = PTR_ERR(filp);
        return NULL;
    }
    return filp;
  */
  return 0;
}

/* Called when device is read.
* filep = pointer to a file
* buffer = pointer to the buffer to which this function writes the data
* len = length of buffer
* offset = offset in buffer
*/
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
  int error_count = 0;
    /*mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());

    ret = vfs_read(file, data, size, &offset);

    set_fs(oldfs);
*/
  //copy_to_user has format ( * to, *from, size), returns 0 on success
  error_count = copy_to_user(buffer, message, size_of_message);
  if (error_count==0) { // success!
    printk(KERN_INFO "blocker: sent %d characters to user\n", size_of_message);
    return (size_of_message=0); // reset position and return 0
  }
  else {
    printk(KERN_INFO "blocker: failed to send %d characters to the user\n", error_count);
    return -EFAULT; // failure!
  }
}

/* Called whenever device is written.
* filep = pointer to file
* buffer = pointer to buffer that contains data to write to the device
* len = length of data
* offset = offset in buffer
*/
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {
   /* mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());

    ret = vfs_write(file, data, size, &offset);

    set_fs(oldfs);
    return ret;*/

  // append received string and its length
  sprintf(message, "%s .(%d letters)", buffer, (int)len);
  // store length of message
  size_of_message = strlen(message);
  printk(KERN_INFO "blocker: received %d characters from the user\n", (int)len);
  return len;
}

/* Called when device is closed/released.
* inodep = pointer to inode
* filep = pointer to a file
*/
static int dev_release(struct inode *inodep, struct file *filep) {
  printk(KERN_INFO "blocker: released\n");
  return 0;
}

module_init(blocker_init);
module_exit(blocker_exit);
