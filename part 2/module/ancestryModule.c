#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <asm/current.h>
#include <asm/errno.h>
#include <asm/uaccess.h>

struct ancestry {
	pid_t ancestors[10];
	pid_t siblings[100];
	pid_t children[100];
};

unsigned long **sys_call_table;

asmlinkage long (*ref_sys_cs3013_syscall2)(unsigned short *target_pid, struct ancestry *response);

asmlinkage long new_sys_cs3013_syscall2(unsigned short *target_pid, struct ancestry *response) {
	pid_t curPID;
	struct task_struct *curTask; // for the current task from target_pid
	struct task_struct *tempTask; // for any task that needs to be added to ancestry
	struct ancestry family; // ancestry returned to copy_to_user
	struct list_head *list; // temp list to parse for each array
	int child;
	int sib;
	int parent;
	
	if (copy_from_user(&family, response, sizeof(family))) {
		return EFAULT;
	}
	
	curTask = pid_task(find_vpid(*target_pid), PIDTYPE_PID); // find task based on pid
	
	curPID = task_pid_nr(curTask);
	printk(KERN_INFO "Targeted Task PID: %d\n", (int) curPID);
	
	child = 0;
	list_for_each(list, &curTask->children) { // list_for_each -> arg1 (cursor), agr2 (list) 
		tempTask = list_entry(list, struct task_struct, sibling); //use sibling here because it gives a linked list of children
		family.children[child] = tempTask->pid;
		printk(KERN_INFO "Targeted Child %d PID: %d\n", (child + 1), (int) family.children[child]);
		child++;
	}
	
	sib = 0;
	list_for_each(list, &curTask->real_parent->children) { // get children of parent (aka siblings)
		tempTask = list_entry(list, struct task_struct, sibling);
		if (tempTask->pid != curPID) { // dont print current task
			family.siblings[sib] = tempTask->pid;
			printk(KERN_INFO "Targeted Sibling %d PID: %d\n", (sib + 1), (int) family.siblings[sib]);
			sib++;
		}
	}
	
	parent = 0;
	tempTask = curTask;
	while(1) {
		tempTask = tempTask->real_parent; // keep going up levels of ancestors
		if (tempTask->pid < 1) {
			break;
		} else {
			family.ancestors[parent] = tempTask->pid;
			printk(KERN_INFO "Targeted Ancestor %d PID: %d\n", (parent + 1), (int) family.ancestors[parent]);
			parent++;
		}
	}
	
	if(copy_to_user(response, &family, sizeof(family))) {
		return EFAULT;
	}
	return 0;
}

// GIVEN CODE

static unsigned long **find_sys_call_table(void) {
  unsigned long int offset = PAGE_OFFSET;
  unsigned long **sct;
  
  while (offset < ULLONG_MAX) {
    sct = (unsigned long **)offset;

    if (sct[__NR_close] == (unsigned long *) sys_close) {
      printk(KERN_INFO "Interceptor: Found syscall table at address: 0x%02lX",
	     (unsigned long) sct);
      return sct;
    }
    
    offset += sizeof(void *);
  }
  
  return NULL;
}

static void disable_page_protection(void) {
  /*
    Control Register 0 (cr0) governs how the CPU operates.

    Bit #16, if set, prevents the CPU from writing to memory marked as
    read only. Well, our system call table meets that description.
    But, we can simply turn off this bit in cr0 to allow us to make
    changes. We read in the current value of the register (32 or 64
    bits wide), and AND that with a value where all bits are 0 except
    the 16th bit (using a negation operation), causing the write_cr0
    value to have the 16th bit cleared (with all other bits staying
    the same. We will thus be able to write to the protected memory.

    It's good to be the kernel!
  */
  write_cr0 (read_cr0 () & (~ 0x10000));
}

static void enable_page_protection(void) {
  /*
   See the above description for cr0. Here, we use an OR to set the 
   16th bit to re-enable write protection on the CPU.
  */
  write_cr0 (read_cr0 () | 0x10000);
}

static int __init interceptor_start(void) {
  /* Find the system call table */
  if(!(sys_call_table = find_sys_call_table())) {
    /* Well, that didn't work. 
       Cancel the module loading step. */
    return -1;
  }
  
  /* Store a copy of all the existing functions */
  ref_sys_cs3013_syscall2 = (void *)sys_call_table[__NR_cs3013_syscall2];

  /* Replace the existing system calls */
  disable_page_protection();

  sys_call_table[__NR_cs3013_syscall2] = (unsigned long *)new_sys_cs3013_syscall2;
  
  enable_page_protection();
  
  /* And indicate the load was successful */
  printk(KERN_INFO "Loaded interceptor!");

  return 0;
}

static void __exit interceptor_end(void) {
  /* If we don't know what the syscall table is, don't bother. */
  if(!sys_call_table)
    return;
  
  /* Revert all system calls to what they were before we began. */
  disable_page_protection();
  sys_call_table[__NR_cs3013_syscall2] = (unsigned long *)ref_sys_cs3013_syscall2;
  enable_page_protection();

  printk(KERN_INFO "Unloaded interceptor!");
}

MODULE_LICENSE("GPL");
module_init(interceptor_start);
module_exit(interceptor_end);