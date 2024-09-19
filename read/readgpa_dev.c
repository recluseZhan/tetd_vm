#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");
#define DEV_ID 232
#define DEVNAME "readgpa_dev"

extern void read_func(void);
extern void write_func(uint64_t gva);
void bind_pa(unsigned long gpa, unsigned long hpa);
void target_dump(unsigned long hpa, unsigned long gpa, unsigned long target_gva, unsigned long flag);
// open dev
static int readgpa_dev_open(struct inode *inode, struct file *filp) 
{   
    return 0; 
} 
// release dev
static int readgpa_dev_release(struct inode *inode, struct file *filp) 
{ 
    return 0; 
} 
// read dev
//static unsigned long gva; // Store the allocated physical address
//static unsigned long gpa;
static ssize_t readgpa_dev_read(struct file *filp, char __user *buf, size_t size, loff_t *offset) 
{   
    /*
    gva = __get_free_page(GFP_KERNEL);
    if (!gva) {
        pr_err("Failed to allocate a 4KB page\n");
        return -ENOMEM;
    }
    pr_info("Allocated physical page at 0x%lx\n", gva);
   
    char *page_ptr = (char *)gva;

    snprintf(page_ptr, PAGE_SIZE, "Hello, this is some data in the allocated page!\n");

    printk(KERN_INFO "Data in page: %s\n", page_ptr);

    gpa = virt_to_phys((void*)gva);  
    unsigned long a[512];
    memset(a,1,512);
    unsigned long b = virt_to_phys((void*)a); 
    bind_pa(b, 0x410e499000);
    */
    unsigned long read_buffer[4];
    unsigned long hpa, gpa, target_gva, flag;

    copy_from_user(read_buffer, buf, size);
    hpa = read_buffer[0];
    gpa = read_buffer[1];
    target_gva = read_buffer[2];
    flag = read_buffer[3];

    target_dump(hpa, gpa, target_gva, flag);
    //read_func();
    return 0;
} 
// write dev
static ssize_t readgpa_dev_write(struct file *filp, const char __user *buf, size_t size, loff_t *offset) 
{  
    uint64_t write_buffer[1];
    uint64_t gva;
    copy_from_user(write_buffer, buf, size);
    gva = write_buffer[0];
    write_func(gva);
    return 0; 
} 

static struct file_operations fops = {
    .owner = THIS_MODULE, 
    .open = readgpa_dev_open, 
    .release= readgpa_dev_release, 
    .read = readgpa_dev_read, 
    .write = readgpa_dev_write, 
}; 

static int __init readgpa_dev_init(void) 
{   
    int ret;
    ret = register_chrdev(DEV_ID,DEVNAME,&fops);
 
    if (ret < 0) {
        printk(KERN_EMERG DEVNAME "can't register major number.\n");
        return ret;
    }
    return 0;
}

static void __exit readgpa_dev_exit(void)
{
    unregister_chrdev(DEV_ID,DEVNAME);
}

module_init(readgpa_dev_init);
module_exit(readgpa_dev_exit);


