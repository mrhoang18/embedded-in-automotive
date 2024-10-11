# embedded-in-automotive
stm32f1, CAN, LIN, AUTOSAR
# LESSON 1: SET UP AND CREATE NEW PROJECTS IN KEILC
<details><summary>Details</summary>
<p>

</p>
</details>




# LESSON 2: GPIO
<details><summary>Details</summary>
<p>

## 1. Thư viện STM32F10x SPL (Standard Peripherals Library)
## 2. Lý thuyết về GPIO

GPIO-General Purpose Input/Output là các chân trên vi điều khiển có thể lập trình để truyền nhận tín 
hiệu với các thiết bị bên ngoài hoặc thực hiện các chức năng giao tiếp 
khác.
 
GPIO hoạt động ở các chế độ sau:

### GPIO Intput

Ở chế độ này, chân GPIO được sử dụng để nhận tín hiệu từ bên ngoài 
vào vi điều khiển.

Các loại Input Mode:

 - **Floating hay High-impedance (thả nổi)**: Chân GPIO không có trạng thái điện áp xác định khi
  không có tín hiệu vào. 

    Ví dụ:

    Mắc một nút nhấn theo kiểu Floating như hình, khi không nhấn nút thì 
    không có tín hiệu vào.

    Khi nhấn nút, VĐK nhận tín hiệu tùy vào chân còn lại của nút nhấn mắc
    với Vcc hay Gnd (trong hình là Gnd, VĐK nhận mức thấp (0)).

    <p align="center">
        <img src="image.png" alt="alt text" width="300">
    </p>

    _Không nên sử dụng mode này! Dễ bị nhiễu lúc không có tín hiệu và tốn năng lượng_

 - **Pull-up (điện trở kéo lên)**: Chân GPIO ở mức cao khi không có tín hiệu đầu vào.
    
    Ví dụ: 
    
    Mắc một nút nhấn theo kiểu pull-up như hình, khi không nhấn nút thì PMOS dẫn,
    VĐK sẽ nhận mức cao (1).
    <p align="center">
        <img src="image-1.png" alt="alt text" width="300">
    </p>

    Ngược lại khi nhấn nút, NMOS dẫn thì VĐK nhận mức thấp (0).
    <p align="center">
        <img src="image-2.png" alt="alt text" width="300">
    </p>

 - **Pull-down (điện trở kéo xuống)**: Chân GPIO ở mức thấp khi không có tín hiệu đầu vào.
    
    Ví dụ: 

    Mắc một nút nhấn theo kiểu pull-down như hình, khi không nhấn nút thì NMOS dẫn,
    VĐK sẽ nhận mức thấp (0).

    <p align="center">
        <img src="image-3.png" alt="alt text" width="300">
    </p>

    Ngược lại khi nhấn nút, PMOS dẫn thì VĐK nhận mức cao (1).
    <p align="center">
        <img src="image-4.png" alt="alt text" width="300">
    </p>

### GPIO Input

Ở chế độ này, chân GPIO được sử dụng để xuất tín hiệu ra bên ngoài từ 
vi điều khiển.

Các loại Output Mode:

 - Push-Pull: Chân GPIO xuất ra có thể là mức cao (1) hoặc mức thấp (0).

    Ví dụ:

    Khi VĐK xuất mức cao (1), PMOS dẫn và chân GPIO sẽ ở mức cao (1).
    <p align="center">
        <img src="image-5.png" alt="alt text" width="200">
    </p>

    Ngược lại, VĐK xuất mức thấp (0), NMOS dẫn và chân GPIO sẽ ở mức thấp (0).
    <p align="center">
        <img src="image-6.png" alt="alt text" width="200">
    </p>

 - Open-Drain: Chân GPIO chỉ  có thể xuất ra mức thấp (0) hoặc ở trạng thái "thả nổi".

    Ví dụ:

    Khi ở mode Open-Drain thì Output Buffer chỉ còn lại một NMOS.

    Khi VĐK xuất mức cao (1), NMOS không dẫn và chân GPIO sẽ thả nổi.
    
    Khi VĐK xuất mức thấp (0), NMOS dẫn và chân GPIO sẽ ở mức thấp (0).

    <p align="center">
        <img src="image-7.png" alt="alt text" width="200">
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

## 3. Thực hành với GPIO

Để sử dụng một ngoại vi bất kì phải trải qua các bước sau:

<p align="center">
    <img src="image-8.png" alt="alt text" width="500">
</p>

Như các thư viện khác (HAL, LL, CMSIS,...), SPL cung cấp các hàm và các định nghĩa giúp việc cấu hình và sử dụng ngoại vi.

### Cấp xung clock cho GPIO

STM32 sử dụng các bus (như AHB, APB1, APB2) để giao tiếp với các ngoại vi như GPIO, UART, I2C, SPI, v.v.

Để sử dụng các ngoại vi này, trước tiên, cần cấp xung (clock) cho bus tương ứng thông qua việc sử dụng các API trong thư viện.

Tra thông tin clock trong Reference (trang 92) và Data sheet (trang 11) để biết được bus nào cần được cấp xung.

<p align="center">
    <img src="image-9.png" alt="alt text" width="300">
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
Việc cấu hình GPIO được thực hiện thông qua việc khai báo và sử dụng struct `GPIO_InitTypeDef`,
 trong đó chứa các tham số cấu hình cho một chân GPIO cụ thể:

 - **GPIO_Pin**: Xác định chân hoặc các chân GPIO muốn cấu hình bằng 
  cách sử dụng các macro như `GPIO_Pin_0`, `GPIO_Pin_1`,... hoặc kết hợp
  các chân bằng toán tử OR `|` nếu muốn cấu hình nhiều chân cùng lúc.

    <p align="center">
        <img src="image-11.png" alt="alt text" width="500">
    </p>

 - **GPIO_Mode**: Xác định mode hoạt động của chân GPIO.

    <p align="center">
        <img src="image-10.png" alt="alt text" width="250">
    </p>

 - **GPIO_Speed**: Chọn tốc độ đáp ứng của chân GPIO.

    <p align="center">
        <img src="image-12.png" alt="alt text" width="200">
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
## 4. Bài tập thực hành (Xem trong folder: lesson-2-GPIO)

</p>
</details>

# LESSON 12: CAN(THEORY)
<details><summary>Details</summary>
<p>

</p>
</details>