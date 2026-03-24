# Cortex-M3 RTOS Projesi

Bu proje, Cortex-M3 mimarisi için geliştirilmiş, katmanlı bir yapıya sahip basit bir Real-Time Operating System (RTOS) çekirdeğidir.

## 📝 Genel Bakış

Bu RTOS, `ready` ve `blocked` durumlarına sahip görevleri (task) yöneten bir Round-Robin zamanlama (scheduling) algoritması kullanır. Projenin temel amacı, modüler bir tasarım oluşturarak birden fazla mikrodenetleyici platformuna kolayca adapte edilebilen bir yapı kurmak ve Cortex-M3 mimarisi üzerindeki pratik bilgiyi artırmaktır.

## ✨ Özellikler

- **Round-Robin Zamanlayıcı:** Görevler arasında adil bir zaman paylaşımı sağlar.
- **Görev Yönetimi:** `Ready` ve `Blocked` durumları ile temel görev yönetimi.
- **Modüler Tasarım:** Donanım bağımlılıklarını en aza indiren katmanlı mimari.
- **Kolay Taşınabilirlik:** Farklı Cortex-M3 tabanlı mikrodenetleyicilere kolayca port edilebilir yapı.
- **Basit ve Anlaşılır Kod:** Eğitim ve öğrenme amaçlı olarak kodun okunabilirliğine önem verilmiştir.

## 🏗️ Mimari

Proje, aşağıdaki katmanlı mimari üzerine kurulmuştur:

- **Application (Uygulama Katmanı):** Kullanıcı uygulamalarının ve görevlerin bulunduğu katmandır. (`app/`)
- **Kernel (Çekirdek Katmanı):** RTOS çekirdeğinin (zamanlayıcı, görev yönetimi vb.) yer aldığı katmandır. (`middleware/`)
- **Port (Taşıma Katmanı):** Çekirdeğin, hedef işlemci mimarisine (Cortex-M3) özgü kodlarını içerir. (`middleware/portable/`)
- **BSP (Board Support Package):** Hedef donanım kartına (örn: STM32F103C8T6) özgü başlangıç kodları ve donanım sürücülerini içerir. (`bsp/`)
- **Drivers (Sürücü Katmanı):** LED gibi çevresel birimlerin sürücülerini barındırır. (`drivers/`)

```mermaid
graph TD
    A[Uygulama (main.c)] --> B{RTOS API (scheduler.h)};
    B --> C[RTOS Çekirdeği (scheduler.c)];
    C --> D{Port Katmanı (port.h)};
    D --> E[Mimar-Özgü Kod (port.c, port_asm.s)];
    C --> F{Sürücüler (led.h)};
    F --> G[Donanım Sürücüleri (led.c)];
    E --> H[Donanım (Cortex-M3)];
    G --> H;
```

## 🛠️ Kurulum ve Derleme
Projeyi derlemek için aşağıdaki adımları izleyin:

1. Gerekli Araçlar:

    - arm-none-eabi-gcc (ARM için C derleyicisi)
    - make
2. Derleme:
Proje kök dizininde terminali açın ve aşağıdaki komutu çalıştırın:

    ```Bash
    make
    ```
Bu komut, build dizini altında projenizi derleyecektir.

## 💻 Desteklenen Donanımlar
STMicroelectronics STM32F103C8T6 ("Blue Pill")

## 🤝 Katkıda Bulunma
Bu proje kişisel gelişim amacıyla oluşturulmuş olsa da, katkıda bulunmak isterseniz "issue" açabilir veya "pull request" gönderebilirsiniz.

## 📄 Lisans
Bu proje MIT Lisansı altında lisanslanmıştır.