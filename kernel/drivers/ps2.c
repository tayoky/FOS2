#include "ps2.h"
#include "irq.h"
#include "panic.h"
#include "print.h"
#include "port.h"
#include "vfs.h"
#include "ringbuf.h"
#include <input.h>

#define PS2_DATA    0x60
#define PS2_COMMAND 0x64
#define PS2_STATUS  0x64

#define PS2_DISABLE_PORT1  0xAD
#define PS2_ENABLE_PORT1   0xAE
#define PS2_DISABLE_PORT2  0xA7
#define PS2_ENABLE_PORT2   0xA8

#define PS2_READ_CONF  0x20
#define PS2_WRITE_CONF 0x60
#define PS2_SET_SCANCODE_SET 0xF0

ring_buffer keyboard_queue;

static void wait_output(){
	while(!(in_byte(PS2_STATUS) & 0x01));
}

static void wait_input(){
	while(in_byte(PS2_STATUS) & 0x02);
}

static void ps2_send_command(uint8_t command){
	wait_input();
	out_byte(PS2_COMMAND,command);
}

static uint8_t ps2_read(void){
	wait_output();
	return in_byte(PS2_DATA);
}

static void ps2_write(uint8_t data){
	wait_input();
	out_byte(PS2_DATA,data);
}

static void keyboard_write(uint8_t data){
	ps2_write(data);
}


char kbd_us[128] = {
	0, 27,
	'1','2','3','4','5','6','7','8','9','0',
	'-','=','\b',
	'\t', /* tab */
	'q','w','e','r','t','y','u','i','o','p','[',']','\n',
	0, /* control */
	'a','s','d','f','g','h','j','k','l',';','\'', '`',
	KEY_SHIFT, /* left shift */
	'\\','z','x','c','v','b','n','m',',','.','/',
	KEY_SHIFT, /* right shift */
	'*',
	0, /* alt */
	' ', /* space */
	KEY_CAPSLOCK, /* caps lock */
	0, /* F1 [59] */
	0, 0, 0, 0, 0, 0, 0, 0,
	0, /* ... F10 */
	0, /* 69 num lock */
	0, /* scroll lock */
	0, /* home */
	0, /* up */
	0, /* page up */
	'-',
	0, /* left arrow */
	0,
	0, /* right arrow */
	'+',
	0, /* 79 end */
	0, /* down */
	0, /* page down */
	0, /* insert */
	0, /* delete */
	0, 0, 0,
	0, /* F11 */
	0, /* F12 */
	0, /* everything else */
};

void keyboard_handler(fault_frame *frame){
	uint8_t scancode = ps2_read();
	if(kbd_us[scancode]){
		ringbuffer_write(&kbd_us[scancode],&keyboard_queue,1);
	} else if(kbd_us[scancode - 0x80]){
		scancode = kbd_us[scancode - 0x80] + 0x80;
		ringbuffer_write(&scancode,&keyboard_queue,1);
	}
}

int64_t kbd_read(vfs_node *node,void *buffer,uint64_t offset,size_t count){
	return ringbuffer_read(buffer,&keyboard_queue,count);
}

device_op kbd_op = {
	.read = kbd_read,
};


void init_ps2(void){
	kstatus("init ps2... ");
	//todo check if ps2 is present

	//disable
	ps2_send_command(PS2_DISABLE_PORT1);
	ps2_send_command(PS2_DISABLE_PORT2);

	//flush output
	in_byte(PS2_DATA);

	//modify configuration
	ps2_send_command(PS2_READ_CONF);
	uint8_t conf = ps2_read();

	conf |= 0x01 ; //Activate irq for port1

	//write conf back
	ps2_send_command(PS2_WRITE_CONF);
	ps2_write(conf);

	//re enable port 1
	ps2_send_command(PS2_ENABLE_PORT1);

	irq_generic_map(keyboard_handler,1);

	//set scancode set 3
	keyboard_write(PS2_SET_SCANCODE_SET);
	keyboard_write(2);

	//init queue
	keyboard_queue = new_ringbuffer(50);

	//create device
	if(vfs_create_dev("dev:/kb0",&kbd_op,NULL)){
		kfail();
		kinfof("fail to create dev dev:/kb0\n");
	}

	kok();
}