## To Do
- error handling - error handlings has two sides one of them is must be implemented into rtos, and if error has been occured in rtos then user must be informed about it.

- logging

- stack owerfow guard note: added for switch contexting, but not for inside the tasks

- doxygen documentation

- impelent drivers 

## First Things
  - add retunr values to funcitons and create enum for return values

GitHub Copilot: Read [](file:///run/media/tura/Ventoy/VSCode/Workspace/stm32f103/README.md)

  # 📋 TinyRTOS - Eklenmesi Gereken Tüm Maddeler

  ## 🔴 **KRİTİK (Bu olmadan sistem çalışmaz/tehlikeli)**

  ### 1. **Task Delay Birim Standardizasyonu**
  - [X] `task_delay_ms(uint32_t ms)` - Milisaniye cinsinden
  - [X] `task_delay_ticks(uint32_t ticks)` - Tick cinsinden
  - [X] Eski `task_delay()` kaldır veya deprecate et
  - [X] **Dosya:** scheduler.h, scheduler.c

  ### 2. **Error Handling & Return Codes**
  - [ ] `sched_add_task()` → return `int8_t` (-1 = full, -2 = invalid)
  - [ ] `sched_init()` → return `int8_t` (-1 = init failed)
  - [ ] `task_delay()` → return `int8_t` (-1 = invalid task)
  - [ ] Enum oluştur: `SCHED_OK`, `SCHED_ERR_MAX_TASKS`, vb.
  - [ ] **Dosya:** scheduler.h, scheduler_priv.h

  ### 3. **Logging/Debug Mekanizması**
  - [ ] UART driver yazılmalı (seri haberleşme)
  - [ ] `debug_printf()` fonksiyonu ekle
  - [ ] Scheduler işlemleri log et (task creation, switch, delay)
  - [ ] **Dosya:** `drivers/uart.c`, `drivers/uart.h`, `drivers/debug.c`

  ### 4. **Stack Overflow Koruması**
  - [X] Stack end'e `0xDEADBEEF` pattern yaz
  - [ ] Scheduler tick'te kontrol et
  - [ ] Overflow durumunda fault handler çağır
  - [ ] **Dosya:** scheduler.c

  ### 5. **Header Guard Hataları Düzelt**
  - [X] scheduler.h → `/* SCHEDULER_H_ */` yaz (şu anda `MAIN_H_`)
  - [X] Tüm header'larda guard kontrol et
  - [X] **Dosya:** scheduler.h, scheduler_config.h, scheduler_priv.h

  ---

  ## 🟠 **ÖNEMLI (Production'a hazır olmak için)**

  ### 6. **Documentation Ekle**
  - [ ] Tüm public fonksiyonlara Doxygen comment'i
    ```c
    /**
    * @brief Task ekle scheduler'a
    * @param task_handler Task fonksiyonu
    * @param tsk_stack_addr Stack başlangıcı
    * @param tsk_stack_size Stack boyutu (word)
    * @return 0: başarılı, -1: MAX_TASKS aşıldı
    */
    ```
  - [ ] **Dosya:** Tüm `.h` dosyaları

  ### 7. **API Tasarımını İyileştir**
  - [ ] `sched_start()` parametresini kaldır (işe yaramıyor)
  - [ ] `sched_get_current_task()` ekle (debug için)
  - [ ] `sched_get_tick_count()` ekle (timing referansı)
  - [ ] `task_yield()` ekle (voluntary context switch)
  - [ ] **Dosya:** scheduler.h, scheduler.c

  ### 8. **Task Durumu Mekanizması**
  - [ ] Task states'ini genişlet:
    ```c
    #define TASK_RUNNING_STATE    0x01
    #define TASK_READY_STATE      0x02
    #define TASK_BLOCKED_STATE    0x04
    #define TASK_SUSPENDED_STATE  0x08
    #define TASK_DELETED_STATE    0x10
    ```
  - [ ] `sched_suspend_task()` ekle
  - [ ] `sched_resume_task()` ekle
  - [ ] **Dosya:** scheduler.h, scheduler_priv.h

  ### 9. **Task Deletion Mekanizması**
  - [ ] `sched_delete_task()` ekle
  - [ ] Silinen task'ın stack'ini mark et
  - [ ] Memory leak olmadığını kontrol et
  - [ ] **Dosya:** scheduler.c, scheduler.h

  ---

  ## 🟡 **ÖNEMLI (İş mantığı/features)**

  ### 10. **Mutex/Semaphore (Senkronizasyon)**
  - [ ] Basit mutex ekle:
    ```c
    typedef struct {
      uint8_t locked;
      uint8_t owner_task;
    } mutex_t;
    ```
  - [ ] `mutex_lock(mutex_t *m)` → blocking
  - [ ] `mutex_unlock(mutex_t *m)`
  - [ ] **Dosya:** `middleware/src/sync.c`, `middleware/include/sync.h`

  ### 11. **Priority Scheduling**
  - [ ] Task struct'una `priority` alanı ekle (0-31, 31=highest)
  - [ ] `sched_add_task_with_priority()` ekle
  - [ ] Scheduling algoritmasını değiştir (round-robin → priority queue)
  - [ ] **Dosya:** scheduler.c, scheduler_priv.h

  ### 12. **Task Statistics**
  - [ ] Task struct'una ekle:
    ```c
    uint32_t task_switches;     // Ne kadar context switch?
    uint32_t total_runtime_ms;  // Toplam koşma süresi
    uint32_t stack_peak_usage;  // En yüksek stack kullanım
    ```
  - [ ] `sched_get_task_stats()` ekle
  - [ ] **Dosya:** scheduler.c, scheduler.h

  ### 13. **Message Queue (Optional, ileri)**
  - [ ] Basit queue ekle (task komunikasyonu için)
  - [ ] `queue_create()`, `queue_send()`, `queue_receive()`
  - [ ] **Dosya:** `middleware/src/queue.c`, `middleware/include/queue.h`

  ---

  ## 🔵 **OPSİYONEL (Gelişmiş)**

  ### 14. **Assert Mekanizması**
  - [ ] Debug build'de assert ekle
  - [ ] Release build'de disable et
  - [ ] **Dosya:** `middleware/include/scheduler_assert.h`

  ### 15. **Interrupt Safety**
  - [ ] ISR'dan task delay() çağrılmasını engelle
  - [ ] ISR'dan scheduler fonksiyonları güvenli mi kontrol et
  - [ ] **Dosya:** scheduler.c, port.c

  ### 16. **Performance Profiling**
  - [ ] Cycle counter ekle (DWT)
  - [ ] Task runtime'ını ölç
  - [ ] **Dosya:** `drivers/profiler.c`

  ### 17. **Unit Tests**
  - [ ] CppUTest framework ekle
  - [ ] `test_scheduler_add_task.c` → test scheduler
  - [ ] `test_task_delay.c` → delay testleri
  - [ ] **Dosya:** `test/` klasörü oluştur

  ### 18. **CI/CD Pipeline**
  - [ ] GitHub Actions `.yml` ekle
  - [ ] Otomatik build
  - [ ] Static analysis (cppcheck)
  - [ ] **Dosya:** `.github/workflows/build.yml`

  ### 19. **GPIO HAL Layer**
  - [ ] GPIO abstraction ekle (platform bağımsız)
  - [ ] `gpio_set()`, `gpio_clear()`, `gpio_toggle()`
  - [ ] **Dosya:** `drivers/hal_gpio.c`, `drivers/hal_gpio.h`

  ### 20. **I2C HAL Layer**
  - [ ] I2C master implementation
  - [ ] `i2c_write()`, `i2c_read()`, `i2c_burst()`
  - [ ] **Dosya:** `drivers/hal_i2c.c`, `drivers/hal_i2c.h`

  ---

  ## 📁 **YENİ DOSYALAR OLUŞTUR**

  ```
  middleware/
  ├── include/
  │   ├── scheduler_assert.h      ← Assert makroları
  │   ├── scheduler_types.h       ← Enum, typedef'ler
  │   └── sync.h                  ← Mutex, semaphore
  ├── src/
  │   ├── sync.c                  ← Senkronizasyon
  │   └── queue.c                 ← Message queue (opsiyonel)
  └── config/
      └── scheduler_config.h      ← Zaten var

  drivers/
  ├── uart.h, uart.c              ← UART driver
  ├── debug.h, debug.c            ← Debug/logging
  ├── hal_gpio.h, hal_gpio.c      ← GPIO abstraction
  ├── hal_i2c.h, hal_i2c.c        ← I2C abstraction
  └── profiler.h, profiler.c      ← Performance (opsiyonel)

  test/
  ├── test_scheduler.c
  ├── test_task_delay.c
  └── test_sync.c

  .github/
  └── workflows/
      └── build.yml               ← CI/CD
  ```

  ---

  ## ✅ **ÖNCELİK SIRASI (Recommended)**

  **Haftası 1 (Kritik):**
  1. Task delay standardizasyonu
  2. Error handling
  3. Logging (UART)
  4. Header guard düzeltme

  **Haftası 2 (Önemli):**
  5. Documentation
  6. API geliştirme
  7. Task suspend/resume
  8. Task deletion

  **Haftası 3-4 (Features):**
  9. Mutex/semaphore
  10. Priority scheduling
  11. Task statistics

  **Sonrasında (Gelişmiş):**
  12. Message queue
  13. GPIO/I2C driver'ları
  14. Unit tests
  15. CI/CD

  ---

  ## 📊 **TOPLAM: 20 Madde**
  - **Kritik:** 5
  - **Önemli:** 4
  - **Features:** 4
  - **Gelişmiş:** 7

  **Tahmini Zaman:**
  - Kritik: ~8 saat
  - Önemli: ~12 saat
  - Features: ~16 saat
  - Gelişmiş: ~20+ saat

  **Toplam: ~56+ saat (1.5 hafta tam zamanla)**