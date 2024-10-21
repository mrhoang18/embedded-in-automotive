# LESSON 01: SET UP AND CREATE NEW PROJECTS IN KEILC
<details><summary>Details</summary>
<p>

</p>
</details>

# LESSON 02: GPIO
<details><summary>Details</summary>
<p>

## 1. Thư viện STM32F10x SPL (Standard Peripherals Library)
## 2. Lý thuyết về GPIO

GPIO-General Purpose Input/Output là các chân trên vi điều khiển có thể lập trình để truyền nhận tín 
hiệu với các thiết bị bên ngoài hoặc thực hiện các chức năng giao tiếp 
khác.
 
GPIO hoạt động ở các chế độ sau:

### GPIO Intput

Ở chế độ này, chân GPIO được sử dụng để nhận tín hiệu từ bên ngoài vào vi điều khiển.

Các loại Input Mode:

 - **Floating hay High-impedance (thả nổi)**: Chân GPIO không có trạng thái điện áp xác định khi không có tín hiệu vào. 

    Ví dụ:

    Mắc một nút nhấn theo kiểu Floating như hình, khi không nhấn nút thì không có tín hiệu vào.

    Khi nhấn nút, VĐK nhận tín hiệu tùy vào chân còn lại của nút nhấn mắc với Vcc hay Gnd (trong hình là Gnd, VĐK nhận mức thấp (0)).

    <p align="center">
        <img src="image/gpio.png" alt="alt text" width="300">
    </p>

    _Không nên sử dụng mode này! Dễ bị nhiễu lúc không có tín hiệu và tốn năng lượng_

 - **Pull-up (điện trở kéo lên)**: Chân GPIO ở mức cao khi không có tín hiệu đầu vào.
    
    Ví dụ: 
    
    Mắc một nút nhấn theo kiểu pull-up như hình, khi không nhấn nút thì PMOS dẫn, VĐK sẽ nhận mức cao (1).
    <p align="center">
        <img src="image/gpio-1.png" alt="alt text" width="300">
    </p>

    Ngược lại khi nhấn nút, NMOS dẫn thì VĐK nhận mức thấp (0).
    <p align="center">
        <img src="image/gpio-2.png" alt="alt text" width="300">
    </p>

 - **Pull-down (điện trở kéo xuống)**: Chân GPIO ở mức thấp khi không có tín hiệu đầu vào.
    
    Ví dụ: 

    Mắc một nút nhấn theo kiểu pull-down như hình, khi không nhấn nút thì NMOS dẫn, VĐK sẽ nhận mức thấp (0).

    <p align="center">
        <img src="image/gpio-3.png" alt="alt text" width="300">
    </p>

    Ngược lại khi nhấn nút, PMOS dẫn thì VĐK nhận mức cao (1).
    <p align="center">
        <img src="image/gpio-4.png" alt="alt text" width="300">
    </p>

### GPIO Input

Ở chế độ này, chân GPIO được sử dụng để xuất tín hiệu ra bên ngoài từ vi điều khiển.

Các loại Output Mode:

 - Push-Pull: Chân GPIO xuất ra có thể là mức cao (1) hoặc mức thấp (0).

    Ví dụ:

    Khi VĐK xuất mức cao (1), PMOS dẫn và chân GPIO sẽ ở mức cao (1).
    <p align="center">
        <img src="image/gpio-5.png" alt="alt text" width="200">
    </p>

    Ngược lại, VĐK xuất mức thấp (0), NMOS dẫn và chân GPIO sẽ ở mức thấp (0).
    <p align="center">
        <img src="image/gpio-6.png" alt="alt text" width="200">
    </p>

 - Open-Drain: Chân GPIO chỉ  có thể xuất ra mức thấp (0) hoặc ở trạng thái "thả nổi".

    Ví dụ:

    Khi ở mode Open-Drain thì Output Buffer chỉ còn lại một NMOS.

    Khi VĐK xuất mức cao (1), NMOS không dẫn và chân GPIO sẽ thả nổi.
    
    Khi VĐK xuất mức thấp (0), NMOS dẫn và chân GPIO sẽ ở mức thấp (0).

    <p align="center">
        <img src="image/gpio-7.png" alt="alt text" width="200">
    </p>

    _Nếu VĐK dùng mosfet thì gọi là Open-Drain, còn nếu dùng BJT gọi là Open-Collector._

### Analog

Ở chế độ này, chân GPIO kết nối với bộ chuyển đổi ADC (analog-to-digital converter) bên trong và cho phép đọc một giá trị đại diện cho điện áp trên chân đó. 

Giá trị này phụ thuộc vào độ phân giải của ADC. 

Ví dụ: ADC 12-bit có thể có các giá trị từ 0 đến 4095. Giá trị này được ánh xạ tới một điện áp nằm trong khoảng từ 0V đến điện áp mà vi điều khiển đang hoạt động (ví dụ, 3.3V). 

Khi một GPIO được cấu hình ở chế độ analog, các điện trở kéo lên/kéo xuống đầu vào sẽ bị vô hiệu (thả nổi).

_Sẽ học kĩ hơn ở bài ADC._

### Alternate Function

Ngoài ba chế độ trên, các chân GPIO còn có thể cung cấp các chức năng thay thế.

Ví dụ các chức năng thay thế: chân Rx/Tx cho giao tiếp UART, chân SDA/SCL cho giao tiếp I2C, v.v.

Để cấu hình cho chức năng thay thế, ta phải tác động lên các thanh ghi cụ thể, sẽ học kĩ hơn ở các bài sau.

## 3. Lập trình với GPIO

Để sử dụng một ngoại vi bất kì phải trải qua các bước sau:

<p align="center">
    <img src="image/gpio-8.png" alt="alt text" width="500">
</p>

Như các thư viện khác (HAL, LL, CMSIS,...), SPL cung cấp các hàm và các định nghĩa giúp việc cấu hình và sử dụng ngoại vi.

### Cấp xung clock cho GPIO

STM32 sử dụng các bus (như AHB, APB1, APB2) để giao tiếp với các ngoại vi như GPIO, UART, I2C, SPI, v.v.

Để sử dụng các ngoại vi này, trước tiên, cần cấp xung (clock) cho bus tương ứng thông qua việc sử dụng các API trong thư viện.

Tra thông tin clock trong Reference (trang 92) và Data sheet (trang 11) để biết được bus nào cần được cấp xung.

<p align="center">
    <img src="image/gpio-9.png" alt="alt text" width="300">
</p>

Trong SPL, để bật xung clock cho ngoại vi GPIO:

```c
void RCC_Config(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
} 
```
 - Tham số thứ 1 là ngoại vi cần cấu hình clock. 

 - Tham số thứ 2 là giá trị quy định cấp (ENABLE) hay ngưng (DISABLE) xung clock cho ngoại vi đó.

### Cấu hình GPIO

Để cấu hình cho chân GPIO:
```c
void GPIO_Config(){
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}
```
Việc cấu hình GPIO được thực hiện thông qua việc khai báo và sử dụng struct `GPIO_InitTypeDef`, trong đó chứa các tham số cấu hình cho một chân GPIO cụ thể:

 - **GPIO_Pin**: Xác định chân hoặc các chân GPIO muốn cấu hình bằng cách sử dụng các macro như `GPIO_Pin_0`, `GPIO_Pin_1`,... hoặc kết hợp các chân bằng toán tử OR `|` nếu muốn cấu hình nhiều chân cùng lúc.

    <p align="center">
        <img src="image/gpio-11.png" alt="alt text" width="500">
    </p>

 - **GPIO_Mode**: Xác định mode hoạt động của chân GPIO.

    <p align="center">
        <img src="image/gpio-10.png" alt="alt text" width="250">
    </p>

 - **GPIO_Speed**: Chọn tốc độ đáp ứng của chân GPIO.

    <p align="center">
        <img src="image/gpio-12.png" alt="alt text" width="200">
    </p>

Hàm khởi tạo GPIO_Init() nhận 2 tham số: 
 - GPIO_TypeDef: Chỉ định cổng GPIO muốn cấu hình (ví dụ: `GPIOA`, `GPIOB`, `GPIOC`,...).
 - &GPIO_InitStruct: Con trỏ đến biến cấu trúc `GPIO_InitTypeDef` chứa các thông số cấu hình.

### Sử dụng GPIO

Một số hàm thao tác với GPIO:

 - Đọc giá trị mức logic (0 hoặc 1) của một chân Input/Output cụ thể trên cổng GPIO.
    ```C
    uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
    uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
    ```
 - Đọc toàn bộ giá trị đầu vào/ra của một cổng GPIO.

    _Giá trị trả về 16-bit, mỗi bit tương ứng với trạng thái của từng chân của cổng._

    ```C
    uint16_t GPIO_ReadInputData(GPIO_TypeDef* GPIOx);
    uint16_t GPIO_ReadOutputData(GPIO_TypeDef* GPIOx);
    ```
 - Đặt mức cao (1) / thấp (0) cho một hoặc nhiều chân output sử dụng OR `|` trên một cổng GPIO.

    ```C
    void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
    void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
    ```
 - Ghi mức logic cụ thể (0 hoặc 1) cho một chân output.

    ```C
    void GPIO_WriteBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BitAction BitVal);
    ```

 - Ghi một giá trị 16-bit trực tiếp cho toàn bộ cổng GPIO, trong đó mỗi bit đại diện cho trạng thái của một chân.
    
    ```C
    void GPIO_Write(GPIO_TypeDef* GPIOx, uint16_t PortVal);
    ```
## 4. Bài tập (Xem trong folder: lesson-2-GPIO)

</p>
</details>


# LESSON 3: INTERRUPTS AND TIMER
<details><summary>Details</summary>
<p>

</p>
</details>

# LESSON 04: CÁC CHUẨN GIAO TIẾP CƠ BẢN
<details><summary>Details</summary>
<p>

## 1. SPI

## Đặc điểm và cấu trúc vật lý

SPI là chuẩn giao tiếp nối tiếp, đồng bộ, cấu trúc Master - Slave, song công (truyền nhận cùng một lúc được), 4 dây.

<p align="center">
    <img src="image/com-1.png" alt="alt text" width="350">
</p>

Bốn chân giao tiếp gồm:
 - SCK (Serial Clock): Master tạo xung tín hiệu SCK và cung cấp cho Slave.
 - MISO (Master Input Slave Output): Tín hiệu tạo bởi Slave và nhận bởi Master.
 - MOSI (Master Output Slave Input): Tín hiệu tạo bởi Master và nhận bởi Slave. 
 - SS (Đôi khi CS- Slave Select/Chip Select): Chọn Slave cụ thể để giao tiếp bằng cách xuất tín hiệu đường SS tương ứng xuống mức 0 (Low). 

## Quá trình truyền nhận


Bắt đầu quá trình, master sẽ kéo chân CS của slave muốn giao tiếp xuống 0 để báo hiệu muốn truyền nhận.
Clock sẽ được cấp bởi master, tùy vào chế độ được cài, với mỗi xungc clock,  1 bit sẽ được truyền từ master đến slave và slave cũng truyền 1 bit cho master.
Các thanh ghi cập nhật giá trị và dịch 1 bit.
Lặp lại quá trình trên đến khi truyền xong 8 bit trong thanh ghi.
Giao tiếp song công.

## Các chế độ hoạt động

## 2. I2C
## Đặc điểm và cấu trúc vật lý

I2C là chuẩn giao tiếp nối tiếp, đồng bộ, cấu trúc Master - Slave, bán song công (chỉ truyền hoặc nhận tại một thời điểm), 2 dây.

## Quá trình truyền nhận
## Các chế độ hoạt động


## 2. UART
## Đặc điểm và cấu trúc vật lý

UART là chuẩn giao tiếp nối tiếp, không đồng bộ, song công, 2 dây


## Quá trình truyền nhận
## Các chế độ hoạt động

</p>
</details>



# LESSON 05: SPI
<details><summary>Details</summary>
<p>
</p>
</details>

# LESSON 06: I2C
<details><summary>Details</summary>
<p>
</p>
</details>

# LESSON 07: UART
<details><summary>Details</summary>
<p>
</p>
</details>

# LESSON 08: EXTERNAL, TIMER, COMMUNICATION INTERRUPTS
<details><summary>Details</summary>
<p>
</p>
</details>

# LESSON 09: ADC
<details><summary>Details</summary>
<p>
</p>
</details>

# LESSON 10: DMA
<details><summary>Details</summary>
<p>
</p>
</details>

# LESSON 11: BOOTLOADER
<details><summary>Details</summary>
<p>
</p>
</details>

# LESSON 12: CAN - CONTROLLER AREA NETWORK
<details><summary>Details</summary>
<p>

## 1. Giao thức CAN 

CAN là một giao thức truyền thông phổ biến trong các hệ thống nhúng, đặc biệt là trong lĩnh vực ô tô.

CAN hỗ trợ mạnh cho những hệ thống điều khiển thời gian thực phân bố (distributed real-time control system).

<p align="center">
    <img src="image/can-1.png" alt="alt text" width="500">
</p>

## 2. Mạng CAN

CAN sử dụng kiến trúc **bus topology**, tất cả các thiết bị đều được kết nối song song vào một cặp dây truyền thông chung được gọi là **CAN Bus** tạo thành **mạng CAN**.

## 3. CAN Node

Mỗi thiết bị trên mạng CAN được gọi là node.

<p align="center">
    <img src="image/can-3.png" alt="alt text" width="800">
</p>

Thành phần trong một node:

 - **Vi điều khiển (Microcontroller)**: Điều khiển hoạt động của node CAN.

    + Đọc và xử lý thông điệp CAN.
    + Tạo ra thông điệp CAN để truyền đi.
	+ Quản lý các khung dữ liệu, bit arbitration và quá trình xử lý lỗi.
	+ Bật/tắt node, reset node khi gặp lỗi bus-off.

 - **CAN Controller**: Được tích hợp bên trong VĐK, có nhiệm vụ xử lý toàn bộ giao tiếp CAN.
	+ Gửi và nhận thông điệp CAN.
	+ Điều khiển truy cập vào bus CAN (arbitration).
	+ Phát hiện và xử lý các lỗi truyền thông CAN.
	+ Kiểm soát việc truyền lại thông điệp khi gặp lỗi.
	+ Cung cấp giao diện giữa các VĐK và bus CAN.

 - **CAN Transceiver**:
    + Chuyển đổi tín hiệu số từ bộ điều khiển CAN thành tín hiệu điện áp dạng differential (CANH và CANL) để gửi lên bus CAN và ngược lại.
    + Đảm bảo tín hiệu truyền và nhận trên bus CAN có độ chính xác và tốc độ cao.

    <p align="center">
        <img src="image/can-2.png" alt="alt text" width="450">
    </p>

 - **Cảm biến (Sensor), cơ cấu chấp hành (Actuator)**:
    + Nhiệt độ, áp suất lốp, tốc độ,...
    + Mở cốp, điều khiển động cơ, bật đèn,...

## 4. CAN Bus

Gồm hai dây tín hiệu: CAN_H (CAN High) và CAN_L (CAN Low). 

Các tín hiệu truyền qua bus CAN là tín hiệu vi sai - chênh lệch điện áp trên hai dây.

<p align="center">
    <img src="image/can-4.png" alt="alt text" width="450">
</p>

CAN bus thường được xoắn 2 dây vào nhau để triệt tiêu nhiễu.

## 5. Dominant và Recessive

Bus CAN định nghĩa hai trạng thái điện áp là: Dominant và Recessive. Tương đương với bit 0 và 1 của các giao thức khác. 

Hai trạng thái này sẽ được xử lý bởi Transceiver. 

Theo tốc độ truyền nhận CAN, ta chia làm hai loại: CAN low speed và CAN high speed. Tương ứng với các giá trị điện áp khác nhau.

 - CAN low speed:
    + Bit Dominant: CAN_H = 4V, CAN_L = 1V.
    + Bit Recessive: CAN_H = 1.75V, CAN_L = 3.25V. 
    <p align="center">
        <img src="image/can-5.png" alt="alt text" width="450">
    </p>

 - CAN high speed:
    + Bit Dominant: CAN_H = 3.5V, CAN_H = 1.5V.
    + Bit Recessive: CAN_H = CAN_L = 2.5V. 
<p align="center">
    <img src="image/can-6.png" alt="alt text" width="450">
</p>

## 5. Các đặc điểm của giao thức CAN 
**Phần này quan trọng!**
### Không tuân theo kiến trúc Master-Slave: 

Tất cả các thiết bị trên bus đều có quyền ngang nhau trong việc truyền dữ liệu mà không cần phải có thiết bị chủ điều khiển. 

Điều này cho phép mạng hoạt động linh hoạt hơn, khi bất kỳ node nào cũng có thể truyền hoặc nhận thông tin bất cứ lúc nào.


### Quá trình tranh chấp quyền gửi (Arbitration)

Tất cả các node trong mạng CAN đều có quyền bắt đầu truyền dữ liệu cùng lúc, nghĩa là chúng có thể bắt đầu phát tín hiệu trên bus mà không cần phải đợi lần lượt. 

Tuy nhiên, đây là chỉ là bước bắt đầu quá trình truyền, và chỉ một node thực sự có thể truyền dữ liệu cuối cùng sau **quá trình Arbitration**:

 - Mỗi thông điệp CAN có một **ID ưu tiên**. Node nào có thông điệp với giá trị ID càng bé thì được độ ưu tiên cao hơn và sẽ chiếm quyền truy cập bus và gửi thông điệp trước.

 - Những node khác có giá trị ID lớn hơn sẽ tự động dừng lại và chờ lượt tiếp theo để gửi thông điệp.

    **Ví dụ: Chọn CAN low speed, có 3 node tham gia gửi.**

    Lưu ý: ID Node-2 > ID Node-1 > ID Node-3 (0x676 > 0x65D > 0x659)

    Đoạn giải thích dưới đây nhằm xoáy sâu vào luận điểm "ID càng bé thì ưu tiên càng cao", thực chất chỉ là kiểm tra từng bit theo giá trị binary, bit ID là Dominant sẽ được ưu tiên hơn.

    <p align="center">
    <img src="image/can-7.png" alt="alt text" width="500">
    </p>

    + Quá trình Arbitration sẽ lần lượt kiểm tra từng bit ID từ MSB tới LSB.
    + Khi đến bit thứ 5, phát hiện bit thứ 5 của ID Node-2 là Recessive nên Node-2 vào hàng chờ và chỉ nghe.
    + Và nếu kiểm tra tiếp thì đến bit thứ 3, phát hiện bit thứ 3 của ID Node-1 là Recessive nên Node-1 tiếp tục vào hàng chờ và chỉ nghe.
    + Node-3 thắng và sẽ được gửi.

 - Quá trình Arbitration diễn ra mà không gây mất dữ liệu hay làm gián đoạn các thiết bị khác, vì thế mạng CAN là một hệ thống non-destructive (không gây mất dữ liệu).

### Broadcast Communication

Khi một node được chọn để gửi thông điệp, thông điệp đó sẽ được gửi đến tất cả các node khác trên bus. 

Tuy nhiên, không phải tất cả các node đều xử lý thông điệp này. Mỗi node sẽ sử dụng bộ lọc để kiểm tra xem thông điệp có phù hợp với mình hay không.

### Giao tiếp song công (Full-duplex Communication)
Mặc dù chỉ sử dụng một bus với hai dây tín hiệu, mạng CAN vẫn cho phép các node vừa gửi vừa nhận dữ liệu đồng thời. Điều này giúp mạng CAN hoạt động hiệu quả và không bị nghẽn khi có nhiều thiết bị cùng giao tiếp.

### Phát hiện và xử lý lỗi tự động

Nếu một node phát hiện ra lỗi trong quá trình truyền hoặc nhận dữ liệu (do nhiễu, mất gói, hoặc lỗi tín hiệu), node đó sẽ gửi một Error Frame để thông báo cho các node khác rằng dữ liệu bị lỗi. Sau đó, thông điệp sẽ được truyền lại.

## 6. Các phiên bản của CAN

Trước khi vào phần CAN Frame phải qua phần này trước vì có sự khác nhau trong Frame của các phiên bản CAN.

Giao thức CAN đã phát triển qua nhiều phiên bản để đáp ứng nhu cầu ngày càng cao trong các ứng đặc biệt là công nghiệp ô tô.

Các phiên bản CAN bao gồm: CAN 2.0A (Standard CAN), CAN 2.0B (Extended CAN), và CAN FD (Flexible Data-rate).

Mỗi phiên bản có những cải tiến để hỗ trợ các yêu cầu khác nhau về độ ưu tiên, dung lượng dữ liệu và tốc độ truyền tải.

## 7. CAN Frame
**Phần này quan trọng!**

_Chú thích: Frame (Khung), Field (Trường)._

Mạng CAN hỗ trợ: Data Frame, Remote Frame, Error Frame và Overload Frame.

### Data Frame

Dùng để truyền dữ liệu.

Có hai phiên bản Data Frame: Khung tiêu chuẩn (CAN 2.0A), khung mở rộng (CAN 2.0B).

**Khung tiêu chuẩn**:

<p align="center">
    <img src="image/can-8.png" alt="alt text" width="800">
</p>

 - **S0F - Start Of Frame**: Luôn là Dominant, bắt đầu thông điệp. 

 - **Identifier**: Xác định mức độ ưu tiên.

 - **RTR - Remote Transmission Request**: Phân biệt Data Frame (=Dominant) và Remote Frame (=Recessive).

 - **IDE - Identifier Extension**: Phân biệt khung tiêu chuẩn (=Dominant) và khung mở rộng (=Recessive).

 - **r0**: Mặc định là Dominant, hiện tại không có chức năng cụ thể, không quan trọng.

 - **DLC - Data Length Code**: Giá trị (0 - 8 theo Binary) ứng với số byte dữ liệu.

 - **Data**: Dữ liệu.

 - **CRC - Cyclic Redundancy Check**: Kiểm tra lỗi thông điệp.

 - **CRC Delimiter**: Luôn là Recessive, dùng để ngăn cách.

 - **ACK - Acknowledge**: Để node nhận ghi vào sau khi kiểm tra lỗi ở CRC, không lỗi = Dominant và có lỗi = Recessive.

 - **ACK Delimiter**: Luôn là Recessive.

 - **EOF - End Of Frame**: 7 bits Recessive, kết thúc thông điệp.

**Khung mở rộng:**

<p align="center">
    <img src="image/can-9.png" alt="alt text" width="1200">
</p>

  - **S0F - Start Of Frame**: Luôn có giá trị Dominant, báo hiệu với các node một thông điệp mới đang bắt đầu. 

 - **Base Identifier**: xác định mức độ ưu tiên.

 - **SRR - Substitute Remote Request**: Độ dài 1 bit, chỉ có ở Extended Frame, giá trị luôn là Recessive.

 - **IDE - Identifier Extension**: Phân biệt khung tiêu chuẩn (=Dominant) và khung mở rộng (=Recessive).

 - **Extended Identifier**: ID mở rộng.

 - **RTR - Remote Transmission Request**: Phân biệt Data Frame (=Dominant) và Remote Frame (=Recessive).

 - **r1, r0**: Mặc định là Dominant, hiện tại không có chức năng cụ thể, không quan trọng.

 - **DLC - Data Length Code**: Giá trị (0 - 8 theo Binary) ứng với số byte dữ liệu.

 - **Data**: Dữ liệu.

 - **CRC - Cyclic Redundancy Check**: Kiểm tra lỗi thông điệp.

 - **CRC Delimiter**: Luôn là Recessive, dùng để ngăn cách.

 - **ACK - Acknowledge**: Để node nhận ghi vào sau khi kiểm tra lỗi ở CRC, không lỗi = Dominant và có lỗi = Recessive.

 - **ACK Delimiter**: Luôn là Recessive.

 - **EOF - End Of Frame**: 7 bits Recessive, kết thúc thông điệp.

### Remote Frame

### Error Frame

Được một node bất kì gửi đi khi phát hiện thông điệp mình vừa nhận không hợp lệ và phải gửi lại.

Error Frame gồm hai phần: 
 - Error Flag là chuỗi từ 6 đến 12 bit dominant, báo hiệu lỗi. 
 - Error Delimiter là chuỗi 8 bit recessive, kết thúc Error Frame.


### Overload Frame

Được một node gửi đi khi nó đang bận xử lý thông điệp hiện tại và yêu cầu các node khác tạm thời dừng truyền cho nó.

</p>
</details>

# LESSON 13: CAN (PRACTICE)
<details><summary>Details</summary>
<p>

Mask và Filter

Mask dùng để lọc bớt ID nhờ cơ chế cổng OR với hai đầu vào bị đảo bởi cổng NOT.

Filer dùng để lọc lại chính xác ID nhờ cổng XOR.

Nếu Mask bằng 0, thì nó cho mọi ID đi qua mặc kệ Filter.

Nếu Mask bằng 1, thì bắt buộc bit của Filter giống với bit ID mới cho qua.

</p>
</details>

# LESSON 14: LIN - LOCAL INTERCONNECT NETWORK
<details><summary>Details</summary>
<p>

## 1. Giao thức LIN

LIN sinh ra nhằm giảm phức tạp và chi phí trong việc truyền thông giữa các thiết bị đơn giản, không cần quá nhanh như điều hòa, đèn. cửa.

CAN và LIN cùng phối hợp trong hệ thống, CAN, LIN giao tiếp với nhau thông qua các Gateway (bộ chuyển đổi giao thức).

<p align="center">
    <img src="image/lin-1.png" alt="alt text" width="600">
</p>

## 2. Đặc điểm

 - Tốc độ truyền thấp: 1 đến 20 kbps phổ biến nhất 19.2 kbps.

 - Mô hình Master - Slave.

 - Giao tiếp không đồng bộ dựa trên UART.

 - Master “quét” yêu cầu tới tất cả các Slave để thăm dò thông tin, Slave chỉ phản hồi khi có yêu cầu từ Master, Master có dữ liệu từ Slave sẽ gửi lên bus CAN để đi tới các LIN khác.
 

## 3. LIN Node

<p align="center">
    <img src="image/lin-3.png" alt="alt text" width="600">
</p>

<p align="center">
    <img src="image/lin-6.png" alt="alt text" width="400">
</p>

Một LIN Node gồm: MCU, LIN Controller, LIN Transceiver, ngoài ra còn có Sensor và Actuator.

LIN dựa trên UART, Chân Tx của MCU nối Tx của Transceiver, tương tự Rx (_Không phải nối chéo_).

LIN hoạt động ở điện áp 12V, Master có nhiều kênh để quản lý nhiều Slave, mỗi một kênh chỉ có một dây tín hiệu.

## 4. LIN Frame

Khung truyền LIN gồm 2 phần lớn: Header và Response.

### Header

<p align="center">
    <img src="image/lin-4.png" alt="alt text" width="800">
</p>

Do Master gửi.

+ **Sync Break**: >=13 bits 0 + 1 bit Delimeter mức cao, bắt đầu của một thông điệp.

+ **Sync Field**: 1 bit Start, 8 bits giá trị cố định 0x55, và 1 bit Stop, đồng bộ hóa tốc độ truyền giữa Master và Slave.

+ **PID Field - Protected Identifier Field**: 6 bits ID, Slave sẽ xem ID có khớp với mình không, nếu có thì xử lý, không thì bỏ qua và 2 bits parity kiểm tra lỗi.

ID quy định độ dài:

| ID Range (Dec) | ID Range (Hex) | Data Length |
|----------------|----------------|-------------|
| 0-31           | 0x00-0x1F      | 2           |
| 32-47          | 0x20-0x2F      | 4           |
| 48-63          | 0x30-0x3F      | 8           |

ID quy định chức năng:

|   ID Range (Dec)   | ID Range (Hex)     | Chức năng                               |
|--------------------|--------------------|-----------------------------------------|
| 0-50               | 0x00-0x3B          | Khung dữ liệu hoặc tín hiệu thông thường|
| 60-61              | 0x3C-0x3D          | Gửi dữ liệu chẩn đoán hoặc cấu hình     |
| 62-63              | 0x3E-0x3F          | Hiện tại chưa có chức năng cụ thể       |

0-50 (0x00-0x3B): Dành cho các khung dữ liệu hoặc tín hiệu thông thường.
60 (0x3C) và 61 (0x3D): Dùng để gửi dữ liệu chẩn đoán hoặc cấu hình.
62 (0x3E) và 63 (0x3F): Dành riêng cho việc mở rộng giao thức trong tương lai.

Cơ chế kiểm tra Parity:

 - P0 = ID0 ⊕ ID1 ⊕ ID2 ⊕ ID4

 - P1 = !(ID1 ⊕ ID3 ⊕ ID4 ⊕ ID5)

### Response

Sau khi node master phát xong header, có hai kiểu Response: 

1. Master yêu cầu dữ liệu từ Slave thì Slave sẽ gửi lại Response cho Master

2. Nếu Master muốn gửi dữ liệu cho Slave thì sẽ gửi kèm Response cho Slave.

<p align="center">
    <img src="image/lin-5.png" alt="alt text" width="800">
</p>

- **Data**: 16 bits đến 64 bits, độ dài do ID quyết định.

- **Checksum**: 8 bits, kiểm tra lỗi.

Có 2 loại phiên bản tính checksum:
 - Classic checksum (LIN 1.x slaves): tính trên Data Field.
 - Enhanced checksum (LIN 2.x slaves): tính trên Data Field, ID Field.

Giả sử có  1 byte ID  và 3 byte data hoặc cả 4 byte data, cách tính như nhau: 0x4A, 0x55, 0x93, 0xE5. _(không tính Start, Stop, Parity)_
 
 - **B1,tính tổng các byte**: 0x4A + 0x55 + 0x93 + 0xE5 = 0x19E (414 thập phân)
 - **B2, nếu tổng lớn hơn 256 thì trừ đi 255**: 0x19E - 0xFF = 0x19.
 - **B3, đảo ngược**: 0x19(0001 1001)-> 0xE6(1110 0110).

## Tổng quan

<p align="center">
    <img src="image/lin-2.png" alt="alt text" width="700">
</p>

</p>
</details>