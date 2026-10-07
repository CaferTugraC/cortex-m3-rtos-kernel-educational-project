# Bilinen Sorunlar

Mevcut koddaki bilinen sorunlar. Proje geliştikçe yeni bulunanlar buraya eklenir. Tasarım sınırlamaları için [README-TR.md](../README-TR.md#tasarım-sınırlamaları) dosyasına bakınız.

**Kurallar:**

- Her sorunun değişmeyen bir ID'si vardır (`KI-01`, `KI-02`, ...). README bu ID'lere link verir.
- Yeni bir sorun listenin sonuna, sıradaki ID ile eklenir. ID'ler yeniden kullanılmaz.
- Düzeltilen sorun silinmez. **Durum** alanı `Düzeltildi (<commit>)` olarak güncellenir.

---

<a id="ki-01"></a>

## KI-01: Kullanılmayan TCB slotları `READY` görünür

- **Durum:** Açık
- **Dosya:** `middleware/scheduler.c` (`update_next_task()`)

`user_tasks[]` statik olduğu için sıfırla başlatılır ve `TASK_READY_STATE` da `0x00`'dır. `update_next_task()` ise `task_count` yerine `MAX_TASKS`'a kadar tarar. Eklenen görev sayısı `MAX_TASKS - 1`'den azsa zamanlayıcı boş bir slotu seçer: PSP 0, `stack_limit` `NULL` olur ve sistem çöker.

**Geçici çözüm:** `MAX_TASKS` değerini tam olarak "kullanıcı görevi sayısı + 1" yapın.

<a id="ki-02"></a>

## KI-02: `sched_init()` port hatalarını yakalamıyor

- **Durum:** Açık
- **Dosya:** `middleware/scheduler.c` (`sched_init()`)

`port_init()` hata durumunda `INVALID_PARAM` döndürür. Bu durum örneğin `TICK_HZ > CONFIG_MAX_TICK_HZ` olduğunda ya da `cpu_clock / TICK_HZ` 24 biti aştığında oluşur. Oysa `sched_init()` yalnızca `ERROR_INIT` değerini kontrol eder ve SysTick ayarlanmasa bile `OK` döner. Bu durumda SysTick kapalı kalır ve `sched_start()` sonrasında hiçbir kullanıcı görevi çalışmaz, sadece idle task çalışır.

**Geçici çözüm:** `TICK_HZ` değerinin `CONFIG_MAX_TICK_HZ`'yi geçmediğinden ve `clock_source / TICK_HZ` değerinin 24 bite sığdığından emin olun.

<a id="ki-03"></a>

## KI-03: SysTick, `sched_start()`'tan önce başlıyor

- **Durum:** Açık
- **Dosyalar:** `middleware/scheduler.c` (`sched_init()`), `middleware/portable/arm_cm3/port.c` (`init_SysTick_timer()`)

SysTick `sched_init()` içinde, idle task oluşturulmadan bile önce çalıştırılır. Ayrıca SysTick'in sayaç register'ı (`SYST_CVR`) sıfırlanmadığı için ilk tick'in ne zaman geleceği belirsizdir. Bu register'ın reset değeri UNKNOWN'dır ve debugger ile yapılan soft reset sonrasında önceki değerini koruyabilir. Bu yüzden:

- Tick idle task eklenmeden gelirse `user_tasks[0].stack_limit` `NULL` olduğundan overflow kontrolü 0x0 adresini okur. Canary uyuşmaz ve sistem sahte bir stack overflow ile durur.
- Tick idle task eklendikten sonra ama `sched_start()`'tan önce gelirse PendSV henüz ayarlanmamış bir PSP ile çalışır. Idle task'ın TCB'si bozulur ve `current_task` değişir.

**Geçici çözüm:** Güvenilir bir geçici çözüm yok. `sched_init()` ile `sched_start()` arasındaki kodu kısa tutmak riski azaltır ama ortadan kaldırmaz.

<a id="ki-04"></a>

## KI-04: Tick taşması gecikmeleri erken bitiriyor

- **Durum:** Açık
- **Dosya:** `middleware/scheduler.c` (`task_delay_tick()`, `unblock_tasks()`)

`block_count = g_tick_count + n` toplamı 32 biti aşarsa `block_count` taşıp küçük bir değere düşer. Bu, sayaç taşmaya yakınken ya da çok büyük bir `n` verildiğinde olur. `block_count <= g_tick_count` koşulu hemen sağlandığı için görev bir sonraki tick'te, yani **erken uyanır**. Sayaç `TICK_HZ = 1000` için ~49,7 günde taşar.

**Geçici çözüm:** Çok büyük `n` değerleri vermeyin. Sistem ~49,7 günden uzun süre çalışacaksa bu sorunu göz önünde bulundurun.

<a id="ki-05"></a>

## KI-05: Kernel, port katmanını `port.h` dışından çağırıyor

- **Durum:** Açık
- **Dosya:** `middleware/scheduler.c` (`check_task_stack_overflow()`)

`__asm volatile("BL UsageFault_Handler");` satırı ARM'a özgüdür ve port katmanındaki fonksiyonu `port.h` arayüzünü atlayarak, isimle çağırır. Bu yüzden kernel başka bir mimariye bu satır değiştirilmeden port edilemez. Inline assembly clobber listesi de tanımlamaz. Handler geri dönmediği için bu şu an zararsızdır.

<a id="ki-06"></a>

## KI-06: Görev stack'leri 8 byte hizalı değil

- **Durum:** Açık
- **Dosyalar:** `app/main.c` (görev stack'leri), `middleware/scheduler.c` (`stack_idle_task`, `sched_add_task()`)

Demo görevlerinin stack'leri ve `stack_idle_task`, `aligned(8)` olmadan tanımlıdır. `sched_add_task()` de stack'in tepesini hizalamaz. Bu yüzden derleyici dizileri `mod 8 = 4` olan bir adrese yerleştirebilir. Bunu `arm-none-eabi-nm -n build/final.elf | grep stack` ile kontrol edebilirsiniz. Bu durumda görevler AAPCS'e aykırı bir SP ile başlar. Bu da `double` / `uint64_t` ve değişken argümanlı fonksiyonlarda (örn. `printf`) sorun çıkarabilir.

**Geçici çözüm:** Kendi görev stack'lerinizi `__attribute__((aligned(8)))` ile ve çift sayıda word olarak tanımlayın. Idle task'ın stack'i için kod değişikliği gerekir.

<a id="ki-07"></a>

## KI-07: LED sürücüsünde race condition var

- **Durum:** Açık
- **Dosya:** `drivers/led.c` (`led_on()`, `led_off()`)

`led_on()` / `led_off()`, `GPIOA_ODR` üzerinde oku-değiştir-yaz yapar ve bu register'ı birden fazla görev kullanır. Okuma ile yazma arasında bir tick gelir ve başka bir görev ODR'yi değiştirirse o değişiklik kaybolur. Atomik çözüm `GPIOA_BSRR` / `GPIOA_BRR` register'larını kullanmaktır.

<a id="ki-08"></a>

## KI-08: `TICK_HZ == clock_source` olursa SysTick hiç kesme üretmez

- **Durum:** Açık
- **Dosya:** `middleware/portable/arm_cm3/port.c` (`init_SysTick_timer()`)

Reload değeri 0 olur ve bu durum kontrol edilmez. SysTick hiç kesme üretmediği için hiçbir kullanıcı görevi çalışmaz.

**Geçici çözüm:** `TICK_HZ` değerini `clock_source` değerinden küçük seçin. Varsayılan ayarlarda bu sorun oluşmaz.

<a id="ki-09"></a>

## KI-09: Handler'larda MSP hizası bozuluyor

- **Durum:** Açık
- **Dosya:** `middleware/portable/arm_cm3/port_asm.s` (`PendSV_Handler`, `switch_sp_to_psp`)

Bu iki fonksiyon stack'e tek bir register itip (`PUSH {LR}`) C fonksiyonu çağırır. Bu, AAPCS'in istediği 8 byte SP hizasını bozar. Çağrılan C fonksiyonları şu an 64-bit veri kullanmadığı için belirti vermez. Çift sayıda register itmek (örn. `PUSH {R4, LR}`) hizayı korur.

<a id="ki-10"></a>

## KI-10: Linker script başlatma bölümlerini tanımlamıyor

- **Durum:** Açık
- **Dosyalar:** `bsp/stm32f103c8t6/stm32f103c8t6_linker_script.ld`, `Makefile`

`.init_array`, `.fini_array` ve `.ARM.exidx` orphan olarak yerleşir ve `Reset_Handler` bunları kopyalamaz. `__libc_init_array()` pratikte hiçbir şey yapmaz. Bu C için zararsızdır, ancak C++ constructor'ları çalışmaz. `-nostartfiles` olmadığı için newlib'in `crt0.o` dosyası da link edilir ve `.data` bölümünde ~1,5 KB RAM kaplar.

<a id="ki-11"></a>

## KI-11: Build sistemi eksik

- **Durum:** Açık
- **Dosya:** `Makefile`

Header bağımlılıkları takip edilmez (`-MMD` yok), bu yüzden bir `.h` dosyası değişince ilgili `.c` dosyaları yeniden derlenmez. `make load` hedefi yalnızca OpenOCD sunucusunu başlatır, karta yükleme yapmaz.

**Geçici çözüm:** Header değişikliğinden sonra `make clean && make` çalıştırın. Yükleme için README'deki OpenOCD komutunu kullanın.
