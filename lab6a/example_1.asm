; C:\USERS\MARTINCHUA\DESKTOP\CPEN412\CPEN412\LAB6A\EXAMPLE_1.C - Compiled by CC68K  Version 5.00 (c) 1991-2005  Peter J. Fondse
; /*
; * EXAMPLE_1.C
; *
; * This is a minimal program to verify multitasking.
; *
; */
; #include <stdio.h>
; #include <Bios.h>
; #include <ucos_ii.h>
; #define STACKSIZE  256
; /* 
; ** Stacks for each task are allocated here in the application in this case = 256 bytes
; ** but you can change size if required
; */
; OS_STK Task1Stk[STACKSIZE];
; OS_STK Task2Stk[STACKSIZE];
; OS_STK Task3Stk[STACKSIZE];
; OS_STK Task4Stk[STACKSIZE];
; /* Prototypes for our tasks/threads*/
; void Task1(void *);	/* (void *) means the child task expects no data from parent*/
; void Task2(void *);
; void Task3(void *);
; void Task4(void *);
; // The global variables
; INT8U led70_global;
; INT8U led89_global;
; INT8U hex12_global;
; INT8U hex34_global;
; INT8U hex56_global;
; /* 
; ** Our main application which has to
; ** 1) Initialise any peripherals on the board, e.g. RS232 for hyperterminal + LCD
; ** 2) Call OSInit() to initialise the OS
; ** 3) Create our application task/threads
; ** 4) Call OSStart()
; */
; void main(void)
; {
       section   code
       xdef      _main
_main:
; // initialise board hardware by calling our routines from the BIOS.C source file
; Init_RS232();
       jsr       _Init_RS232
; Init_LCD();
       jsr       _Init_LCD
; /* display welcome message on LCD display */
; Oline0("Altera DE1/68K");
       pea       @exampl~1_1.L
       jsr       _Oline0
       addq.w    #4,A7
; Oline1("Micrium uC/OS-II RTOS");
       pea       @exampl~1_2.L
       jsr       _Oline1
       addq.w    #4,A7
; OSInit();		// call to initialise the OS
       jsr       _OSInit
; /* 
; ** Now create the 4 child tasks and pass them no data.
; ** the smaller the numerical priority value, the higher the task priority 
; */
; OSTaskCreate(Task1, OS_NULL, &Task1Stk[STACKSIZE], 12);     
       pea       12
       lea       _Task1Stk.L,A0
       add.w     #512,A0
       move.l    A0,-(A7)
       clr.l     -(A7)
       pea       _Task1.L
       jsr       _OSTaskCreate
       add.w     #16,A7
; OSTaskCreate(Task2, OS_NULL, &Task2Stk[STACKSIZE], 11);     // highest priority task
       pea       11
       lea       _Task2Stk.L,A0
       add.w     #512,A0
       move.l    A0,-(A7)
       clr.l     -(A7)
       pea       _Task2.L
       jsr       _OSTaskCreate
       add.w     #16,A7
; // OSTaskCreate(Task3, OS_NULL, &Task3Stk[STACKSIZE], 13);
; // OSTaskCreate(Task4, OS_NULL, &Task4Stk[STACKSIZE], 14);	    // lowest priority task
; OSStart();  // call to start the OS scheduler, (never returns from this function)
       jsr       _OSStart
       rts
; }
; /*
; ** IMPORTANT : Timer 1 interrupts must be started by the highest priority task 
; ** that runs first which is Task2
; */
; void Task1(void *pdata)
; {
       xdef      _Task1
_Task1:
       link      A6,#0
; // PortA = 0; // [7:0]
; // PortB = 0; // [9:8]
; for (;;) {
Task1_1:
; printf("This is Task #1: Incrementing LEDs\n");
       pea       @exampl~1_3.L
       jsr       _printf
       addq.w    #4,A7
; PortA = led70_global;
       move.b    _led70_global.L,4194304
; PortB = led89_global;
       move.b    _led89_global.L,4194306
; if(led70_global == 0xff){
       move.b    _led70_global.L,D0
       and.w     #255,D0
       cmp.w     #255,D0
       bne.s     Task1_4
; led70_global = 0;
       clr.b     _led70_global.L
; if (led89_global == 0x3){
       move.b    _led89_global.L,D0
       cmp.b     #3,D0
       bne.s     Task1_6
; led89_global = 0;
       clr.b     _led89_global.L
       bra.s     Task1_7
Task1_6:
; }
; else{
; led89_global++;
       addq.b    #1,_led89_global.L
Task1_7:
       bra.s     Task1_5
Task1_4:
; }
; }
; else led70_global++;
       addq.b    #1,_led70_global.L
Task1_5:
; OSTimeDly(50);
       pea       50
       jsr       _OSTimeDly
       addq.w    #4,A7
       bra       Task1_1
; }
; }
; /*
; ** Task 2 below was created with the highest priority so it must start timer1
; ** so that it produces interrupts for the 100hz context switches
; */
; void Task2(void *pdata)
; {
       xdef      _Task2
_Task2:
       link      A6,#0
; // must start timer ticker here 
; Timer1_Init() ;      // this function is in BIOS.C and written by us to start timer      
       jsr       _Timer1_Init
; for (;;) {
Task2_1:
; printf("....This is Task #2: Incrementing HEX\n");
       pea       @exampl~1_4.L
       jsr       _printf
       addq.w    #4,A7
; if (hex34_global == 0xff) HEX_C = hex56_global++;
       move.b    _hex34_global.L,D0
       and.w     #255,D0
       cmp.w     #255,D0
       bne.s     Task2_4
       move.b    _hex56_global.L,D0
       addq.b    #1,_hex56_global.L
       move.b    D0,4194324
Task2_4:
; if (hex12_global == 0xff)
       move.b    _hex12_global.L,D0
       and.w     #255,D0
       cmp.w     #255,D0
       bne.s     Task2_6
; HEX_B = hex34_global++;
       move.b    _hex34_global.L,D0
       addq.b    #1,_hex34_global.L
       move.b    D0,4194322
Task2_6:
; HEX_A = hex12_global++;
       move.b    _hex12_global.L,D0
       addq.b    #1,_hex12_global.L
       move.b    D0,4194320
; OSTimeDly(10);
       pea       10
       jsr       _OSTimeDly
       addq.w    #4,A7
       bra       Task2_1
; }
; }
; // void Task3(void *pdata)
; // {
; //     for (;;) {
; //        printf("........This is Task #3\n");
; //        OSTimeDly(40);
; //     }
; // }
; // void Task4(void *pdata)
; // {
; //     for (;;) {
; //        printf("............This is Task #4\n");
; //        OSTimeDly(50);
; //     }
; // }
       section   const
@exampl~1_1:
       dc.b      65,108,116,101,114,97,32,68,69,49,47,54,56,75
       dc.b      0
@exampl~1_2:
       dc.b      77,105,99,114,105,117,109,32,117,67,47,79,83
       dc.b      45,73,73,32,82,84,79,83,0
@exampl~1_3:
       dc.b      84,104,105,115,32,105,115,32,84,97,115,107,32
       dc.b      35,49,58,32,73,110,99,114,101,109,101,110,116
       dc.b      105,110,103,32,76,69,68,115,10,0
@exampl~1_4:
       dc.b      46,46,46,46,84,104,105,115,32,105,115,32,84
       dc.b      97,115,107,32,35,50,58,32,73,110,99,114,101
       dc.b      109,101,110,116,105,110,103,32,72,69,88,10,0
       section   bss
       xdef      _Task1Stk
_Task1Stk:
       ds.b      512
       xdef      _Task2Stk
_Task2Stk:
       ds.b      512
       xdef      _Task3Stk
_Task3Stk:
       ds.b      512
       xdef      _Task4Stk
_Task4Stk:
       ds.b      512
       xdef      _led70_global
_led70_global:
       ds.b      1
       xdef      _led89_global
_led89_global:
       ds.b      1
       xdef      _hex12_global
_hex12_global:
       ds.b      1
       xdef      _hex34_global
_hex34_global:
       ds.b      1
       xdef      _hex56_global
_hex56_global:
       ds.b      1
       xref      _Init_LCD
       xref      _Timer1_Init
       xref      _Init_RS232
       xref      _OSInit
       xref      _OSStart
       xref      _OSTaskCreate
       xref      _Oline0
       xref      _Oline1
       xref      _OSTimeDly
       xref      _printf
