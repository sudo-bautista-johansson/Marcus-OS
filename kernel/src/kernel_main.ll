; ModuleID = 'kryo'
source_filename = "kryo.kyo"
target datalayout = "e-m:e-p:64:64-i64:64-n32:64-S128"
target triple = "x86_64-pc-none-elf"

@.str.0 = private unnamed_addr constant [42 x i8] c"========================================\0A\00"
@.str.1 = private unnamed_addr constant [28 x i8] c"          MARCUS OS KERNEL\0A\00"
@.str.2 = private unnamed_addr constant [43 x i8] c"========================================\0A\0A\00"
@.str.3 = private unnamed_addr constant [22 x i8] c"Architecture: x86_64\0A\00"
@.str.4 = private unnamed_addr constant [16 x i8] c"Language: Kryo\0A\00"
@.str.5 = private unnamed_addr constant [10 x i8] c"Boot: OK\0A\00"
@.str.6 = private unnamed_addr constant [9 x i8] c"CPU: OK\0A\00"
@.str.7 = private unnamed_addr constant [31 x i8] c"Memory: identity mapped 2 MiB\0A\00"
@.str.8 = private unnamed_addr constant [15 x i8] c"Serial: COM1\0A\0A\00"
@.str.9 = private unnamed_addr constant [31 x i8] c"Physical memory: allocator OK\0A\00"
@.str.10 = private unnamed_addr constant [35 x i8] c"Physical memory: allocator FAILED\0A\00"
@.str.11 = private unnamed_addr constant [28 x i8] c"Virtual memory: mapping OK\0A\00"
@.str.12 = private unnamed_addr constant [32 x i8] c"Virtual memory: mapping FAILED\0A\00"
@.str.13 = private unnamed_addr constant [36 x i8] c"Memory protection: NX/read-only OK\0A\00"
@.str.14 = private unnamed_addr constant [27 x i8] c"Memory protection: FAILED\0A\00"
@.str.15 = private unnamed_addr constant [35 x i8] c"Address spaces: switch/restore OK\0A\00"
@.str.16 = private unnamed_addr constant [24 x i8] c"Address spaces: FAILED\0A\00"
@.str.17 = private unnamed_addr constant [28 x i8] c"Kernel heap: allocation OK\0A\00"
@.str.18 = private unnamed_addr constant [32 x i8] c"Kernel heap: allocation FAILED\0A\00"
@.str.19 = private unnamed_addr constant [30 x i8] c"Scheduler: runnable queue OK\0A\00"
@.str.20 = private unnamed_addr constant [19 x i8] c"Scheduler: FAILED\0A\00"
@.str.21 = private unnamed_addr constant [28 x i8] c"Threads: context switch OK\0A\00"
@.str.22 = private unnamed_addr constant [17 x i8] c"Threads: FAILED\0A\00"
@.str.23 = private unnamed_addr constant [30 x i8] c"Processes: address spaces OK\0A\00"
@.str.24 = private unnamed_addr constant [19 x i8] c"Processes: FAILED\0A\00"
@.str.25 = private unnamed_addr constant [25 x i8] c"IPC: channel message OK\0A\00"
@.str.26 = private unnamed_addr constant [13 x i8] c"IPC: FAILED\0A\00"
@.str.27 = private unnamed_addr constant [44 x i8] c"Storage/VFS: MarcusFS read/write/delete OK\0A\00"
@.str.28 = private unnamed_addr constant [21 x i8] c"Storage/VFS: FAILED\0A\00"
@.str.29 = private unnamed_addr constant [30 x i8] c"Framebuffer: VGA graphics OK\0A\00"
@.str.30 = private unnamed_addr constant [21 x i8] c"Framebuffer: FAILED\0A\00"
@.str.31 = private unnamed_addr constant [34 x i8] c"Networking: Ethernet/IPv4/UDP OK\0A\00"
@.str.32 = private unnamed_addr constant [20 x i8] c"Networking: FAILED\0A\00"
@.str.33 = private unnamed_addr constant [29 x i8] c"NIC: e1000 MMIO initialized\0A\00"
@.str.34 = private unnamed_addr constant [25 x i8] c"NIC: e1000 init failed: \00"
@.str.35 = private unnamed_addr constant [2 x i8] c"\0A\00"
@.str.36 = private unnamed_addr constant [40 x i8] c"Networking state: ARP/socket tables OK\0A\00"
@.str.37 = private unnamed_addr constant [26 x i8] c"Networking state: FAILED\0A\00"
@.str.38 = private unnamed_addr constant [27 x i8] c"NIC: e1000 TX/RX rings OK\0A\00"
@.str.39 = private unnamed_addr constant [26 x i8] c"NIC: e1000 rings FAILED: \00"
@.str.40 = private unnamed_addr constant [30 x i8] c"NIC: DMA layout/alignment OK\0A\00"
@.str.41 = private unnamed_addr constant [24 x i8] c"NIC: DMA layout FAILED\0A\00"
@.str.42 = private unnamed_addr constant [34 x i8] c"Networking: ARP TX/RX on wire OK\0A\00"
@.str.43 = private unnamed_addr constant [31 x i8] c"Networking: ARP TX/RX FAILED: \00"
@.str.44 = private unnamed_addr constant [6 x i8] c"NIC: \00"
@.str.45 = private unnamed_addr constant [24 x i8] c"Interrupts: PIC/PIT OK\0A\00"
@.str.46 = private unnamed_addr constant [10 x i8] c"IDT: OK\0A\0A\00"
@.str.47 = private unnamed_addr constant [31 x i8] c"MARCUS OS kernel initialized.\0A\00"

declare void @kernel_puts(i8* %arg0)

declare void @kernel_idt_init()

declare void @kernel_interrupts_init()

declare i32 @kernel_wait_for_tick()

declare i32 @kernel_scheduler_test()

declare i32 @kernel_context_switch_test()

declare i32 @kernel_process_test()

declare i32 @kernel_ipc_test()

declare i32 @kernel_filesystem_test()

declare i32 @kernel_framebuffer_test()

declare i32 @kernel_network_test()

declare i32 @kernel_nic_probe_test()

declare i32 @kernel_nic_init_stage()

declare i8* @kernel_nic_stage_message()

declare i32 @kernel_network_state_test()

declare i32 @kernel_nic_ring_test()

declare i8* @kernel_nic_ring_stage_message()

declare i32 @kernel_nic_dma_layout_test()

declare i8* @kernel_nic_register_state()

declare i32 @kernel_network_wire_test()

declare i8* @kernel_nic_wire_stage_message()

declare void @kernel_halt()

declare void @kernel_memory_init(i64 %arg0)

declare i64 @kernel_page_alloc()

declare void @kernel_page_free(i64 %arg0)

declare i32 @kernel_virtual_mapping_test()

declare i32 @kernel_virtual_protection_test()

declare i32 @kernel_address_space_test()

declare i64 @kernel_heap_alloc(i64 %arg0)

declare void @kernel_heap_free(i64 %arg0, i64 %arg1)

define dso_local void @kernel_main() {
  entry:
  %0 = getelementptr [42 x i8], [42 x i8]* @.str.0, i64 0, i64 0
  call void @kernel_puts(i8* %0)
  %1 = getelementptr [28 x i8], [28 x i8]* @.str.1, i64 0, i64 0
  call void @kernel_puts(i8* %1)
  %2 = getelementptr [43 x i8], [43 x i8]* @.str.2, i64 0, i64 0
  call void @kernel_puts(i8* %2)
  %3 = getelementptr [22 x i8], [22 x i8]* @.str.3, i64 0, i64 0
  call void @kernel_puts(i8* %3)
  %4 = getelementptr [16 x i8], [16 x i8]* @.str.4, i64 0, i64 0
  call void @kernel_puts(i8* %4)
  %5 = getelementptr [10 x i8], [10 x i8]* @.str.5, i64 0, i64 0
  call void @kernel_puts(i8* %5)
  %6 = getelementptr [9 x i8], [9 x i8]* @.str.6, i64 0, i64 0
  call void @kernel_puts(i8* %6)
  %7 = getelementptr [31 x i8], [31 x i8]* @.str.7, i64 0, i64 0
  call void @kernel_puts(i8* %7)
  %8 = getelementptr [15 x i8], [15 x i8]* @.str.8, i64 0, i64 0
  call void @kernel_puts(i8* %8)
  call void @kernel_memory_init(i32 20480)
  %9 = call i64 @kernel_page_alloc()
  %10 = alloca i64
  store i64 %9, i64* %10
  %11 = call i64 @kernel_page_alloc()
  %12 = alloca i64
  store i64 %11, i64* %12
  %13 = load i64, i64* %10
  %14 = icmp ne i64 %13, 0
  %15 = load i64, i64* %12
  %16 = icmp ne i64 %15, 0
  %17 = and i1 %14, %16
  %18 = load i64, i64* %10
  %19 = load i64, i64* %12
  %20 = icmp ne i64 %18, %19
  %21 = and i1 %17, %20
  br i1 %21, label %L1, label %L2
  L1:
    %22 = getelementptr [31 x i8], [31 x i8]* @.str.9, i64 0, i64 0
    call void @kernel_puts(i8* %22)
    %23 = load i64, i64* %10
    call void @kernel_page_free(i64 %23)
    %24 = load i64, i64* %12
    call void @kernel_page_free(i64 %24)
    br label %L3
  L2:
    %25 = getelementptr [35 x i8], [35 x i8]* @.str.10, i64 0, i64 0
    call void @kernel_puts(i8* %25)
    br label %L3
  L3:
  %26 = call i32 @kernel_virtual_mapping_test()
  %27 = icmp ne i32 %26, 0
  br i1 %27, label %L4, label %L5
  L4:
    %28 = getelementptr [28 x i8], [28 x i8]* @.str.11, i64 0, i64 0
    call void @kernel_puts(i8* %28)
    br label %L6
  L5:
    %29 = getelementptr [32 x i8], [32 x i8]* @.str.12, i64 0, i64 0
    call void @kernel_puts(i8* %29)
    br label %L6
  L6:
  %30 = call i32 @kernel_virtual_protection_test()
  %31 = icmp ne i32 %30, 0
  br i1 %31, label %L7, label %L8
  L7:
    %32 = getelementptr [36 x i8], [36 x i8]* @.str.13, i64 0, i64 0
    call void @kernel_puts(i8* %32)
    br label %L9
  L8:
    %33 = getelementptr [27 x i8], [27 x i8]* @.str.14, i64 0, i64 0
    call void @kernel_puts(i8* %33)
    br label %L9
  L9:
  %34 = call i32 @kernel_address_space_test()
  %35 = icmp ne i32 %34, 0
  br i1 %35, label %L10, label %L11
  L10:
    %36 = getelementptr [35 x i8], [35 x i8]* @.str.15, i64 0, i64 0
    call void @kernel_puts(i8* %36)
    br label %L12
  L11:
    %37 = getelementptr [24 x i8], [24 x i8]* @.str.16, i64 0, i64 0
    call void @kernel_puts(i8* %37)
    br label %L12
  L12:
  %38 = call i64 @kernel_heap_alloc(i32 128)
  %39 = alloca i64
  store i64 %38, i64* %39
  %40 = load i64, i64* %39
  %41 = icmp ne i64 %40, 0
  br i1 %41, label %L13, label %L14
  L13:
    %42 = getelementptr [28 x i8], [28 x i8]* @.str.17, i64 0, i64 0
    call void @kernel_puts(i8* %42)
    %43 = load i64, i64* %39
    call void @kernel_heap_free(i64 %43, i32 128)
    br label %L15
  L14:
    %44 = getelementptr [32 x i8], [32 x i8]* @.str.18, i64 0, i64 0
    call void @kernel_puts(i8* %44)
    br label %L15
  L15:
  call void @kernel_idt_init()
  %45 = call i32 @kernel_scheduler_test()
  %46 = icmp ne i32 %45, 0
  br i1 %46, label %L16, label %L17
  L16:
    %47 = getelementptr [30 x i8], [30 x i8]* @.str.19, i64 0, i64 0
    call void @kernel_puts(i8* %47)
    br label %L18
  L17:
    %48 = getelementptr [19 x i8], [19 x i8]* @.str.20, i64 0, i64 0
    call void @kernel_puts(i8* %48)
    br label %L18
  L18:
  %49 = call i32 @kernel_context_switch_test()
  %50 = icmp ne i32 %49, 0
  br i1 %50, label %L19, label %L20
  L19:
    %51 = getelementptr [28 x i8], [28 x i8]* @.str.21, i64 0, i64 0
    call void @kernel_puts(i8* %51)
    br label %L21
  L20:
    %52 = getelementptr [17 x i8], [17 x i8]* @.str.22, i64 0, i64 0
    call void @kernel_puts(i8* %52)
    br label %L21
  L21:
  %53 = call i32 @kernel_process_test()
  %54 = icmp ne i32 %53, 0
  br i1 %54, label %L22, label %L23
  L22:
    %55 = getelementptr [30 x i8], [30 x i8]* @.str.23, i64 0, i64 0
    call void @kernel_puts(i8* %55)
    br label %L24
  L23:
    %56 = getelementptr [19 x i8], [19 x i8]* @.str.24, i64 0, i64 0
    call void @kernel_puts(i8* %56)
    br label %L24
  L24:
  %57 = call i32 @kernel_ipc_test()
  %58 = icmp ne i32 %57, 0
  br i1 %58, label %L25, label %L26
  L25:
    %59 = getelementptr [25 x i8], [25 x i8]* @.str.25, i64 0, i64 0
    call void @kernel_puts(i8* %59)
    br label %L27
  L26:
    %60 = getelementptr [13 x i8], [13 x i8]* @.str.26, i64 0, i64 0
    call void @kernel_puts(i8* %60)
    br label %L27
  L27:
  %61 = call i32 @kernel_filesystem_test()
  %62 = icmp ne i32 %61, 0
  br i1 %62, label %L28, label %L29
  L28:
    %63 = getelementptr [44 x i8], [44 x i8]* @.str.27, i64 0, i64 0
    call void @kernel_puts(i8* %63)
    br label %L30
  L29:
    %64 = getelementptr [21 x i8], [21 x i8]* @.str.28, i64 0, i64 0
    call void @kernel_puts(i8* %64)
    br label %L30
  L30:
  %65 = call i32 @kernel_framebuffer_test()
  %66 = icmp ne i32 %65, 0
  br i1 %66, label %L31, label %L32
  L31:
    %67 = getelementptr [30 x i8], [30 x i8]* @.str.29, i64 0, i64 0
    call void @kernel_puts(i8* %67)
    br label %L33
  L32:
    %68 = getelementptr [21 x i8], [21 x i8]* @.str.30, i64 0, i64 0
    call void @kernel_puts(i8* %68)
    br label %L33
  L33:
  %69 = call i32 @kernel_network_test()
  %70 = icmp ne i32 %69, 0
  br i1 %70, label %L34, label %L35
  L34:
    %71 = getelementptr [34 x i8], [34 x i8]* @.str.31, i64 0, i64 0
    call void @kernel_puts(i8* %71)
    br label %L36
  L35:
    %72 = getelementptr [20 x i8], [20 x i8]* @.str.32, i64 0, i64 0
    call void @kernel_puts(i8* %72)
    br label %L36
  L36:
  %73 = call i32 @kernel_nic_probe_test()
  %74 = icmp ne i32 %73, 0
  br i1 %74, label %L37, label %L38
  L37:
    %75 = getelementptr [29 x i8], [29 x i8]* @.str.33, i64 0, i64 0
    call void @kernel_puts(i8* %75)
    br label %L39
  L38:
    %76 = getelementptr [25 x i8], [25 x i8]* @.str.34, i64 0, i64 0
    call void @kernel_puts(i8* %76)
    %77 = call i8* @kernel_nic_stage_message()
    call void @kernel_puts(i8* %77)
    %78 = getelementptr [2 x i8], [2 x i8]* @.str.35, i64 0, i64 0
    call void @kernel_puts(i8* %78)
    br label %L39
  L39:
  %79 = call i32 @kernel_network_state_test()
  %80 = icmp ne i32 %79, 0
  br i1 %80, label %L40, label %L41
  L40:
    %81 = getelementptr [40 x i8], [40 x i8]* @.str.36, i64 0, i64 0
    call void @kernel_puts(i8* %81)
    br label %L42
  L41:
    %82 = getelementptr [26 x i8], [26 x i8]* @.str.37, i64 0, i64 0
    call void @kernel_puts(i8* %82)
    br label %L42
  L42:
  %83 = call i32 @kernel_nic_ring_test()
  %84 = icmp ne i32 %83, 0
  br i1 %84, label %L43, label %L44
  L43:
    %85 = getelementptr [27 x i8], [27 x i8]* @.str.38, i64 0, i64 0
    call void @kernel_puts(i8* %85)
    br label %L45
  L44:
    %86 = getelementptr [26 x i8], [26 x i8]* @.str.39, i64 0, i64 0
    call void @kernel_puts(i8* %86)
    %87 = call i8* @kernel_nic_ring_stage_message()
    call void @kernel_puts(i8* %87)
    %88 = getelementptr [2 x i8], [2 x i8]* @.str.35, i64 0, i64 0
    call void @kernel_puts(i8* %88)
    br label %L45
  L45:
  %89 = call i32 @kernel_nic_dma_layout_test()
  %90 = icmp ne i32 %89, 0
  br i1 %90, label %L46, label %L47
  L46:
    %91 = getelementptr [30 x i8], [30 x i8]* @.str.40, i64 0, i64 0
    call void @kernel_puts(i8* %91)
    br label %L48
  L47:
    %92 = getelementptr [24 x i8], [24 x i8]* @.str.41, i64 0, i64 0
    call void @kernel_puts(i8* %92)
    br label %L48
  L48:
  %93 = call i32 @kernel_network_wire_test()
  %94 = icmp ne i32 %93, 0
  br i1 %94, label %L49, label %L50
  L49:
    %95 = getelementptr [34 x i8], [34 x i8]* @.str.42, i64 0, i64 0
    call void @kernel_puts(i8* %95)
    br label %L51
  L50:
    %96 = getelementptr [31 x i8], [31 x i8]* @.str.43, i64 0, i64 0
    call void @kernel_puts(i8* %96)
    %97 = call i8* @kernel_nic_wire_stage_message()
    call void @kernel_puts(i8* %97)
    %98 = getelementptr [2 x i8], [2 x i8]* @.str.35, i64 0, i64 0
    call void @kernel_puts(i8* %98)
    br label %L51
  L51:
  %99 = getelementptr [6 x i8], [6 x i8]* @.str.44, i64 0, i64 0
  call void @kernel_puts(i8* %99)
  %100 = call i8* @kernel_nic_register_state()
  call void @kernel_puts(i8* %100)
  %101 = getelementptr [2 x i8], [2 x i8]* @.str.35, i64 0, i64 0
  call void @kernel_puts(i8* %101)
  call void @kernel_interrupts_init()
  %102 = call i32 @kernel_wait_for_tick()
  %103 = icmp ne i32 %102, 0
  br i1 %103, label %L52, label %L53
  L52:
    %104 = getelementptr [24 x i8], [24 x i8]* @.str.45, i64 0, i64 0
    call void @kernel_puts(i8* %104)
    br label %L54
  L53:
    br label %L54
  L54:
  %105 = getelementptr [10 x i8], [10 x i8]* @.str.46, i64 0, i64 0
  call void @kernel_puts(i8* %105)
  %106 = getelementptr [31 x i8], [31 x i8]* @.str.47, i64 0, i64 0
  call void @kernel_puts(i8* %106)
  call void @kernel_halt()
  ret void
}

define dso_local i32 @main() {
  entry:
  call void @kernel_main()
  ret i32 0
}

