#define rVIC0IRQSTATUS 0xF2000000
#define rVIC0ADDRESS   0xF2000F00

.text
.extern printf
.extern uart_init
.extern Start_ArmBoot
.global _start
_start:

	//uart_init();
	bl uart_init
	
	// 关闭看门狗
	ldr	r0, =0xE2700000			
	mov	r1, #0
	str	r1, [r0]
	
	//cpsr[7]清零，也代表说允许IRQ异常中断
	mrs r1,cpsr
	bic r1,r1,#0x80
	msr cpsr,r1
	
	//安装IRQ异常处理函数到0xD0037418
	ldr r0,=HandlerIRQ
	ldr r1,=0xD0037418
	str r0,[r1]

	// 设置栈，以便调用c函数
	ldr	sp, =0x31000000
	
	//Start_ArmBoot();
	bl Start_ArmBoot	
	//while(1)
halt:
	b halt 

HandlerIRQ:
	//lr == while(1);
	sub lr,lr,#4
	//保护现场
	stmfd sp!,{r0-r12,lr}
	
	//保存当前的恢复现场函数地址到lr
	ldr lr,=return_irq
	
	//判断中断源
	ldr r0,=rVIC0IRQSTATUS
	ldr r1,[r0]
	
	//如果有中断产生的话，r1的值是非零
	cmp r1,#0
	bne vic0_handler

return_irq:	
	//恢复现场,^表示spsr手动恢复到cpsr
	ldmfd sp!,{r0-r12,pc}^

vic0_handler:
	ldr r0,=rVIC0ADDRESS
	//跳转到当前有效的中断服务函数当中
	ldr pc,[r0]

.end

