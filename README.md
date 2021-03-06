CoOS - RTOS, by CooCox(http://www.coocox.org)

http://www.coocox.org/coos.html
https://github.com/coocox/coos


General Information
-------------------------------------------------------------------------------

CooCox CoOS is an embedded real-time multi-task OS specially for ARM Cortex M series.
-

Features
-------------------------------------------------------------------------------

- Free and open real-time Operating System 
- Specially designed for Cortex-M series 
- Scalable, minimum system kernel is only 974Bytes 
- Adaptive Task Scheduling Algorithm. 
- Supports preemptive priority and round-robin 
- Interrupt latency is next to 0 
- Stack overflow detection option
- Semaphore, Mutex, Flag, Mailbox and Queue for communication & synchronisation 
- Supports the platforms of ICCARM, ARMCC, GCC


Technical Data
-------------------------------------------------------------------------------
Time Specifications:

          N Function                         Time ( No Robin/Robin ) 
Create defined task, no task switch          5.3us / 5.8us 
Create defined task, switch task             7.5us / 8.6us 
Delete task (ExitTask)                       4.8us / 5.2us 
Task switch (SwitchContext)                  1.5us / 1.5 us 
Task switch (upon set flag)                  7.5us / 8.1us 
Task switch (upon sent semaphore)            6.3us / 7.0us 
Task switch (upon sent mail)                 6.1us / 7.1us 
Task switch (upon sent queue)                7.0us / 7.6us 
Set Flag (no task switch)                    1.3us / 1.3us 
Send semaphore (no task switch)              1.6us / 1.6us 
Send mail (no task switch)                   1.5us / 1.5us 
Send queue (no task switch)                  1.8us / 1.8us 
Maximum interrupt lockout for IRQ ISR's      0 / 0 

Space Specifications:

       Description                    Space 
RAM Space for Kernel                  168 Bytes 
Code Space for Kernel                 974 Bytes 
RAM Space for a Task                  TaskStackSize  + 24 Bytes(MIN) 
                                      TaskStackSize + 48 Bytes(MAX) 
RAM Space for a Mailbox               16 Bytes 
RAM Space for a Semaphore             16 Bytes 
RAM Space for a Queue                 32 Bytes 
RAM Space for a Mutex                 8   Bytes 
RAM Space for a User Timer            24 Bytes 

  
User's Guide
-------------------------------------------------------------------------------

We have supplied user's guide to help you to use CooCox CoOS more easily and confidently, which includes:
-  Chapter 1 CooCox CoOS Overview 
-  Chapter 2 Task Management 
-  Chapter 3 Time Management 
-  Chapter 4 Memory Management 
-  Chapter 5 Intertask Synchronization & Communication 
-  Chapter 6 API Reference 
To get the details, please see the guide in the /CoOS/Document/CooCox_CoOS_User_Guide.pdf.


Repository Information
-------------------------------------------------------------------------------

The contents of the repository can be divided into four parts:
- CoOS source code that ported to various MCU platforms 
- Examples based on CoOS, such as LPC1766_LwIP and so on 
- Protocol Stack based on CoOS, such as USB stack and so on
- Some manufacturers library 

Contribution
-------------------------------------------------------------------------------

Wish to be a contributor of CoOS? You can:
1. Port CoOS to a new ARM series
2. Improve CoOS and correction bugs
3. Develop middlewares such as gui tcp-ip based on CoOS
4. Add a CoOS example to a series chips.

Please see the guide in the /Document/.

Repository Layout
-------------------------------------------------------------------------------

/CoOS/
    CoOS CooCox CoOS's source code.
	CooCox CoOS's port for Cortex M. It includes the source code for Cortex M 
	architecture with three edition: MDK, IAR, GCC.
	CooCox CoOS's document TERMS AND CONDITIONS
    
    kernel/
        CooCox CoOS's source code. 
                
    portable/
        CooCox CoOS's port for Cortex M. It includes the source code for Cortex M 
	    architecture with three edition: MDK, IAR, GCC.
        
        GCC/
            port for GCC.
                
        IAR/
            port for IAR.

        Keil/
            port for Keil.
            
    Document/
        Some document.
        
/Document/
    Some document.
    
/LICENSE
    Licensing and copyright information.
    
/Example/
    Examples of CoOS for each manufacturer.
    
    ST/
        Examples of CoOS for ST.
		
		STM32F1xx/
		    BSPLIB/
			    SRC/
				INC/
				
			STM32F103RB_CoOS/
			    A example name
				
				IAR/
				
			        APP/
				    Project/
				    ...
			    ...
			
			...
		...   
		
	Atmel/
        Examples of CoOS for Atmel.
		
		AT91SAM3Ux/
		    
		    BSPLIB/
			    SRC/
				INC/
				
			AT91SAM3U4E/
			
			...	
		...	
			
	Energy/
        Examples of CoOS for Energy.
		
		EFM32G8xx/
		    
		    BSPLIB/
			    SRC/
				INC/
				
			EFM32G890F128/
			
			...	
		...	
		
	TI/
        Examples of CoOS for TI.
		
		LM3S9000/
		    
		    BSPLIB/
			    SRC/
				INC/
				
			LM3S9B96/
			
			...	
		...
		
	Nuvoton/
        Examples of CoOS for Nuvoton.
		
		NUC1xx/
		    
		    BSPLIB/
			    SRC/
				INC/
				
			NUC140/
			
			...	
		...
			
	NXP/
        Examples of CoOS for NXP.
		
		LPC17xx/
		    
		    BSPLIB/
			    SRC/
				INC/
				
			LPC1766/
			
			...	
		...
		
	Holteck/
        Examples of CoOS for Holteck.  
        
		HT32F125x/
		    
		    BSPLIB/
			    SRC/
				INC/
				
			HT32F1253/
			
			...	
		...
        	
    ...
        
/README
    This file.
    
/Tools/
    Some useful tools.
        
        
    
    
        
        

