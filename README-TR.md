# Cortex‑M3 RTOS Kernel (STM32F103C8T6) — Minimal Scheduler

Bu repo, ARM **Cortex‑M3** tabanlı bir MCU üzerinde çalışan **minimal bir RTOS çekirdeği / scheduler** denemesidir. Proje; **SysTick** ile sistem tick üretir, **PendSV** ile context switch (bağlam değiştirme) yapar ve task’ları **PSP (Process Stack Pointer)** üzerinden koşturur.

---

## İçindekiler
- [Özellikler](#özellikler)
- [Hedef Donanım](#hedef-donanım)
- [Klasör Yapısı](#klasör-yapısı)
- [Hızlı Başlangıç](#hızlı-başlangıç)
- [API Kullanımı (Özet)](#api-kullanımı-özet)
- [Konfigürasyon](#konfigürasyon)
- [Scheduler Davranışı](#scheduler-davranışı)
- [Stack Kullanımı ve Stack Overflow Koruması](#stack-kullanımı-ve-stack-overflow-koruması)
- [Bilinen Kısıtlar / TODO](#bilinen-kısıtlar--todo)
- [Katkı](#katkı)
- [Lisans](#lisans)

---

## Özellikler

- **Task ekleme**: `sched_add_task()` ile handler + stack pointer + stack size vererek task oluşturma
- **Idle task**: Scheduler başlatılırken idle task otomatik eklenir
- **Tick tabanlı zaman**: `SysTick_Handler` ile tick artışı
- **Gecikme (delay) / bloklama**:
  - `task_delay_tick(tick)`
  - `task_delay_ms(ms)`
- **Context switch**:
  - Tick sonunda `PendSV` pendedilerek context switch tetiklenir
  - PendSV içinde **R4–R11** yazılım ile save/restore yapılır
- **Stack overflow kontrolü (sentinel)**:
  - Stack’in belirli bir noktasına `0xDEADBEEF` yazılarak kontrol yapılır
  - Overflow şüphesinde `UsageFault_Handler` yoluna gidilir

### Henüz yok / hedeflenebilir
- Task öncelikleri (priority) ve priority-based scheduling
- Mutex / semaphore / queue gibi senkronizasyon primitive’leri
- Heap/allocator ve dinamik task oluşturma stratejileri
- Tickless idle / güç yönetimi
- Zamanlayıcılar (software timers)
- Unit test / CI

---

## Hedef Donanım

- **CPU mimarisi**: ARM Cortex‑M3
- **BSP klasörü**: `bsp/stm32f103c8t6/` (STM32F103C8T6 hedefi)
- Örnek driver: `drivers/led.*` STM32F103 register seviyesinde GPIOA üzerinden örnek LED kontrolü içerir.

> Not: CPU clock bilgisi `sched_init(clock_source)` parametresi ile verilir. Buraya gerçek CPU frekansını (Hz) vermen gerekir (ör. `72000000`).

---

## Klasör Yapısı

- `middleware/`
  - `scheduler.c/.h`: çekirdek scheduler ve task yönetimi
  - `portable/arm_cm3/`: Cortex‑M3 port katmanı
    - `port.c/.h`: SysTick init, PendSV tetikleme, hook altyapısı
    - `port_asm.s`: PendSV handler + PSP/MSP geçişleri + fault handler köprüleri
- `common/`
  - `scheduler_config.h`: temel konfigürasyonlar
  - `return_enum.h`: durum kodları
- `drivers/`
  - `led.c/.h`: örnek LED sürücüsü (STM32F103)
- `bsp/`
  - kart/MCU’ya özel dosyalar (startup/linker vb. burada konumlanır)

---

## Hızlı Başlangıç

> Bu repo bare‑metal bir projedir. Derlemek/çalıştırmak için tipik olarak şunlar gerekir:
- `arm-none-eabi-gcc` toolchain
- (opsiyonel) OpenOCD + ST-Link/J-Link vb. debugger
- Uygun **startup** ve **linker script** (BSP altında)

### Genel akış
1. BSP + startup + linker ile projeyi ayağa kaldır
2. `sched_init(cpu_clock_hz)` çağır
3. Task’ları `sched_add_task()` ile ekle (idle task zaten init sırasında eklenir)
4. `sched_start()` ile scheduler’ı başlat

> Önemli: Task stack’leri kullanıcı tarafından ayrılır ve `sched_add_task()`’e verilir.

---

## API Kullanımı

Header: `middleware/scheduler.h`

### Init
- `System_Status_t sched_init(uint32_t clock_source);`
  - `clock_source`: CPU clock (Hz). `0` olamaz.

### Task ekleme
- `System_Status_t sched_add_task(void (*task_handler)(void), uint32_t *tsk_stack_addr, uint16_t tsk_stack_size);`
  - `task_handler`: task fonksiyonu (sonsuz döngü içermesi beklenir)
  - `tsk_stack_addr`: stack array başlangıcı
  - `tsk_stack_size`: stack uzunluğu (word cinsinden)

### Scheduler start
- `void sched_start(void);`

### Tick ve delay
- `uint32_t sched_get_tick(void);`
- `void task_delay_tick(uint32_t tick_count);`
- `void task_delay_ms(uint32_t ms);`

---

## Konfigürasyon

`common/scheduler_config.h`:

- `MAX_TASKS`
  - **Idle task dahil** maksimum task sayısı (ör: 3 kullanıcı task’ı + idle = 4)
- `TICK_HZ`
  - SysTick frekansı
- `CONFIG_MAX_TICK_HZ`
  - Güvenlik limiti
- `MIN_STACK_SIZE`
  - Bir task için minimum stack (word)

---

## Scheduler Davranışı

- `SysTick_Handler` çalıştığında:
  - Tick hook üzerinden global tick artırılır
  - Bloklu task’lar tick’e göre çözülür (unblock)
  - PendSV tetiklenerek context switch yapılır

- Task seçimi:
  - Mevcut kod akışında **basit bir round‑robin** benzeri seçim yapılır.
  - Idle task (task 0) “hiçbir task hazır değilse” çalışır.

---

## Stack Kullanımı ve Stack Overflow Koruması

- Cortex‑M3 task stack frame’i (xPSR, PC, LR, R0‑R12 vb.) `sched_add_task()` içinde hazırlanır.
- Stack overflow için “sentinel” yaklaşımı kullanılır:
  - Stack’in belirli bir noktasına `0xDEADBEEF` yazılır
  - Zamanla bozulursa veya PSP sınırı ihlal edilirse overflow şüphesi oluşur
  - Bu durumda `UsageFault_Handler` yoluna gidilir

---

## Katkı

PR ve issue’lar için:
- Hata/eksik bildiriminde: hedef kart, clock, kullanılan toolchain ve yeniden üretim adımlarını ekleyin.
- Yeni özellik eklerken: mümkünse `common/` altında konfigürasyon seçenekleri ile gelin.

---

## Lisans

Bu repo için lisans bilgisi bu dosyada belirtilmemiş olabilir. Eğer açık kaynak yayımlanacaksa uygun bir lisans dosyası (`LICENSE`) eklemeniz önerilir.