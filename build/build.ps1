$ErrorActionPreference = "Stop"

$root = Split-Path -Parent $PSScriptRoot
$out = Join-Path $PSScriptRoot "out"
$clang = (Get-Command clang).Source
$lld = (Get-Command ld.lld).Source
$objcopy = (Get-Command llvm-objcopy).Source
$kryo = (Get-Command kryo).Source
New-Item -ItemType Directory -Force $out | Out-Null

Get-ChildItem "$root\kernel\src" -Filter *.kyo -File | ForEach-Object {
    & $kryo check $_.FullName
    if ($LASTEXITCODE -ne 0) { throw "Kryo check failed for $($_.Name)." }
}
& $kryo build --target=x86_64-marcus-os "$root\kernel\src\kernel_main.kyo" -o "$out\kernel_main.o"
if ($LASTEXITCODE -ne 0) { throw "Kryo freestanding target build failed." }
if (!(Test-Path "$out\kernel_main.o")) { throw "Kryo did not emit the target object." }
& $clang --target=i386-pc-none-elf -c "$root\kernel\src\boot\boot.S" -o "$out\boot.o"
if ($LASTEXITCODE -ne 0) { throw "Bootloader compilation failed." }
& $clang --target=x86_64-pc-none-elf -ffreestanding -fno-pie -fno-stack-protector -mno-red-zone -mno-sse -mno-mmx -mno-80387 -O2 -c "$root\kernel\src\kernel.c" -o "$out\kernel.o"
if ($LASTEXITCODE -ne 0) { throw "Kernel compilation failed." }
& $clang --target=x86_64-pc-none-elf -ffreestanding -fno-pie -fno-stack-protector -mno-red-zone -mno-sse -mno-mmx -mno-80387 -O2 -c "$root\kernel\src\interrupts.c" -o "$out\interrupts.o"
if ($LASTEXITCODE -ne 0) { throw "Interrupt support compilation failed." }
& $clang --target=x86_64-pc-none-elf -ffreestanding -fno-pie -fno-stack-protector -mno-red-zone -mno-sse -mno-mmx -mno-80387 -O2 -c "$root\kernel\src\memory.c" -o "$out\memory.o"
if ($LASTEXITCODE -ne 0) { throw "Memory support compilation failed." }
& $clang --target=x86_64-pc-none-elf -ffreestanding -fno-pie -fno-stack-protector -mno-red-zone -mno-sse -mno-mmx -mno-80387 -O2 -c "$root\kernel\src\scheduler.c" -o "$out\scheduler.o"
if ($LASTEXITCODE -ne 0) { throw "Scheduler compilation failed." }
& $clang --target=x86_64-pc-none-elf -ffreestanding -fno-pie -fno-stack-protector -mno-red-zone -mno-sse -mno-mmx -mno-80387 -O2 -c "$root\kernel\src\thread.c" -o "$out\thread.o"
if ($LASTEXITCODE -ne 0) { throw "Thread support compilation failed." }
& $clang --target=x86_64-pc-none-elf -ffreestanding -fno-pie -fno-stack-protector -mno-red-zone -mno-sse -mno-mmx -mno-80387 -O2 -c "$root\kernel\src\process.c" -o "$out\process.o"
if ($LASTEXITCODE -ne 0) { throw "Process support compilation failed." }
& $clang --target=x86_64-pc-none-elf -ffreestanding -fno-pie -fno-stack-protector -mno-red-zone -mno-sse -mno-mmx -mno-80387 -O2 -c "$root\kernel\src\ipc.c" -o "$out\ipc.o"
if ($LASTEXITCODE -ne 0) { throw "IPC support compilation failed." }
& $clang --target=x86_64-pc-none-elf -ffreestanding -fno-pie -fno-stack-protector -mno-red-zone -mno-sse -mno-mmx -mno-80387 -O2 -c "$root\kernel\src\storage.c" -o "$out\storage.o"
if ($LASTEXITCODE -ne 0) { throw "Storage driver compilation failed." }
& $clang --target=x86_64-pc-none-elf -ffreestanding -fno-pie -fno-stack-protector -mno-red-zone -mno-sse -mno-mmx -mno-80387 -O2 -c "$root\kernel\src\filesystem.c" -o "$out\filesystem.o"
if ($LASTEXITCODE -ne 0) { throw "Filesystem compilation failed." }
& $clang --target=x86_64-pc-none-elf -ffreestanding -fno-pie -fno-stack-protector -mno-red-zone -mno-sse -mno-mmx -mno-80387 -O2 -c "$root\kernel\src\graphics.c" -o "$out\graphics.o"
if ($LASTEXITCODE -ne 0) { throw "Graphics compilation failed." }
& $clang --target=x86_64-pc-none-elf -ffreestanding -fno-pie -fno-stack-protector -mno-red-zone -mno-sse -mno-mmx -mno-80387 -O2 -c "$root\kernel\src\networking.c" -o "$out\networking.o"
if ($LASTEXITCODE -ne 0) { throw "Networking compilation failed." }
& $clang --target=x86_64-pc-none-elf -ffreestanding -fno-pie -fno-stack-protector -mno-red-zone -mno-sse -mno-mmx -mno-80387 -O2 -c "$root\kernel\src\arch\x86_64\interrupts.S" -o "$out\interrupts_stub.o"
if ($LASTEXITCODE -ne 0) { throw "Interrupt stub compilation failed." }
& $clang --target=x86_64-pc-none-elf -ffreestanding -fno-pie -fno-stack-protector -mno-red-zone -mno-sse -mno-mmx -mno-80387 -O2 -c "$root\kernel\src\arch\x86_64\context.S" -o "$out\context.o"
if ($LASTEXITCODE -ne 0) { throw "Context switch compilation failed." }
& $lld -m elf_x86_64 -T "$root\kernel\linker.ld" --oformat=binary "$out\kernel_main.o" "$out\kernel.o" "$out\interrupts.o" "$out\memory.o" "$out\scheduler.o" "$out\thread.o" "$out\process.o" "$out\ipc.o" "$out\storage.o" "$out\filesystem.o" "$out\graphics.o" "$out\networking.o" "$out\interrupts_stub.o" "$out\context.o" -o "$out\kernel.bin"
if ($LASTEXITCODE -ne 0) { throw "Kernel linking failed." }
& $lld -m elf_i386 -T "$root\kernel\boot.ld" "$out\boot.o" -o "$out\boot.elf"
if ($LASTEXITCODE -ne 0) { throw "Boot sector linking failed." }
& $objcopy -O binary "$out\boot.elf" "$out\boot.bin"
if ($LASTEXITCODE -ne 0) { throw "Boot sector extraction failed." }

$boot = [IO.File]::ReadAllBytes("$out\boot.bin")
if ($boot.Length -ne 512) {
    throw "Boot sector must be exactly 512 bytes; got $($boot.Length)."
}
$kernel = [IO.File]::ReadAllBytes("$out\kernel.bin")
$sectorSize = 512
$maxKernelBytes = 128 * $sectorSize
if ($kernel.Length -gt $maxKernelBytes) {
    throw "Kernel exceeds the bootloader limit of $maxKernelBytes bytes."
}
$paddedLength = [int]([Math]::Ceiling($kernel.Length / $sectorSize) * $sectorSize)
$padded = New-Object byte[] $paddedLength
[Array]::Copy($kernel, $padded, $kernel.Length)
[IO.File]::WriteAllBytes("$out\kernel.padded.bin", $padded)
$diskKernel = New-Object byte[] $maxKernelBytes
[Array]::Copy($padded, $diskKernel, $padded.Length)
$filesystem = New-Object byte[] (7 * $sectorSize)
$magic = [Text.Encoding]::ASCII.GetBytes("MFS1")
[Array]::Copy($magic, 0, $filesystem, 0, $magic.Length)
[Array]::Copy([BitConverter]::GetBytes([uint32]1), 0, $filesystem, 4, 4)
[Array]::Copy([BitConverter]::GetBytes([uint32]130), 0, $filesystem, 8, 4)
[Array]::Copy([BitConverter]::GetBytes([uint32]132), 0, $filesystem, 12, 4)
$fileName = [Text.Encoding]::ASCII.GetBytes("WELCOME.TXT")
[Array]::Copy($fileName, 0, $filesystem, 512, $fileName.Length)
[Array]::Copy([BitConverter]::GetBytes([uint32]132), 0, $filesystem, 512 + 24, 4)
$fileContent = [Text.Encoding]::ASCII.GetBytes("MARCUS OS storage online`n")
[Array]::Copy([BitConverter]::GetBytes([uint32]$fileContent.Length), 0, $filesystem, 512 + 28, 4)
[Array]::Copy([BitConverter]::GetBytes([uint32]3), 0, $filesystem, 512 + 32, 4)
[Array]::Copy($fileContent, 0, $filesystem, 1536, $fileContent.Length)
[IO.File]::WriteAllBytes("$out\marcus-os.img", $boot + $diskKernel + $filesystem)
Write-Output "Built $out\marcus-os.img ($($boot.Length + $diskKernel.Length + $filesystem.Length) bytes)"