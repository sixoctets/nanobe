# Nanobe
Seemingly smallest co-operating programming...

## Evaluate

### Hardware requirements
- One of the following Development Kits
  - Nordic Semiconductor nRF51 PCA10028 DK
  - Nordic Semiconductor nRF52 PCA10040 DK
- Your favourite logic analysis hardware for execution profiling

### Software requirements
- Your favourite PC or Mac with Linux distribution or MacOS (with HomeBrew)
- arm-none-eabi- toolchain and make installed
- nrfjprog tool

### Building

#### For nRF51 Series
In the makefile.inc, ensure the following:

Assembler CPU option
```
ASFLAGS = \
	-mcpu=cortex-m0 \
```

C compiler CPU option
```
CFLAGS = \
	-mcpu=cortex-m0 \
```

SoC
```
	-DNRF51 \
```

UART pin
```
	-DUART=8 \
```

#### For nRF52 Series
In the makefile.inc, ensure the following:

Assembler CPU option
```
ASFLAGS = \
	-mcpu=cortex-m4 \
```

C compiler CPU option
```
CFLAGS = \
	-mcpu=cortex-m4 \
```

SoC
```
	-DNRF52 \
```

UART pin
```
	-DUART=5 \
```

### Flashing
```
# nrfjprog -e
# nrfjprog --program app/app_profile.hex
```

## Code Snippets
Call stack definitions
```
static uint8_t __noinit isr_stack[256];
static uint8_t __noinit main_stack[512];
static uint8_t __noinit nanobe_0_stack[256];

void * const isr_stack_top = isr_stack + sizeof(isr_stack);
void * const main_stack_top = main_stack + sizeof(main_stack);
```

A *Nanobe*
```
void nanobe_0(void)
{
	while(1) {
		DEBUG_PIN_OFF(13);
		DEBUG_PIN_ON(13);
	}
}
```

*Nanobe* Injection Stub

Injection function is an execution context injected at the point of *Nanobe*/
program interruption such that an Interrupt Service Routine in exception mode
returns to the injection function in CPU program mode before finally returning
to the interrupted *Nanobe*/program.

```
void nanobe_injection(void)
{
	DEBUG_PIN_ON(14);
	nanobe_sched_yield();
	DEBUG_PIN_OFF(14);
	return;
}

```

A *Nanobe* initialization
```
	nanobe_sp= _nanobe_init(nanobe_0,
				nanobe_0_stack + sizeof(nanobe_0_stack));
	nanobe_sched_enqueue(nanobe_sp);
```

## CPU Utilization
- Prototyping profile numbers on ARM Cortex M0 16 MHz, Nordic Semiconductor
  nRF51 Series
- *Nanobe* to *Nanobe* switch, actual: *10.5* us
- Pre-emption to *Nanobe* switch (switch call in injection), actual:
  *18.1875* us
  - PendSV to Injection (includes setting PendSV), actual: *7.6875* us
  - IRQ to Injection, actual: *7.4375* us
  - Injection to Nanobe switch, actual: *10.5* us
  - 10.5 + 7.6875 = 18.1875 us
- No IRQ disable, no non-deterministic ISR latencies

## Programming model
Programming model of a Nanobe application comprises of
- Interrupt Service Routines (ISR)
- Bottom-halves (BH)
- Program

### Program
Application executes as Nanobe programs. A program being the normal execution
state of any CPU.

*Nanobe* is an experimental implementation of co-operative
program mode execution context switcher.

### Bottom-Halves
These are execution context at priorities higher than a Nanobe program but
low enough to provide ISR low latencies.

*Mayfly* are race-to-idle execution contexts. These are implemented as
context-safe IRQ lockless interfaces. These support many-to-one caller-callee
execution delegations.

## Nanobe Goals
CPU is leading consumer of power/current in a System-on-Chip. Hardware
execution context switching has not been able to suffice complex embedded
system design and execution context management, and leading to wasted CPU
energy. Hence, the need for a very light-weight efficient and flexible software
context switcher.

With the popularity of multi-CPU System-on-Chip, a scalable programmers model
is the need of the day. It is also necessary to maintain inter-component
resource utilization independence, requiring one module/program from not
interfering with others.

### Goals
- Deterministic ISR latencies
- Near bare-metal programming models
- Soft Real Time execution contexts
- Race-to-idle model
- Flexible program call stack configuration

## Nanobe execution context model

### Nanobe
- Co-operative
- Pre-emptible
- Independent call stacks

### Mayfly
- ISR bottom-halves
- Race-to-idle
- Interrupts *Nanobe*
- Either shares a common *Mayfly* call stack or has independent individual
  call stacks

### Interrupt Service Routine
- Interrupts lower priority level *ISR*s, any *Mayfly* and any *Nanobe*
- Injects an optional *Mayfly* into *Nanobe*
- Race to Idle
- Interrupts a *Mayfly* and/or *Nanobe*
- Either shares a common *ISR* call stack or has independent individual *ISR*
  call stack

## Nanobe call stacks
- *Nanobe* call stack shall be independent of *Mayfly* or *ISR* implementation

- Minimal call stacks
  - 1 *Nanobe* call stack

- Typical Call Stacks
  - n *Nanobe* Call Stacks
  - 1 *ISR* Call Stack

- Flexible Call Stacks
  - n Nanobe Call Stacks
  - m Mayfly Call Stacks
  - p ISR Call Stacks

## TODO
- Support for x86, RISC-V and other architectures
- Nanobe program synchronization features
- Integration of *Mayfly*
- Time awareness, system timer concept
- Time Slicing implementation
- Soft Real Time execution model integration
