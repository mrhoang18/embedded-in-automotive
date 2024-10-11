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
## 2. GPIO-General Purpose Input/Output 

GPIO là các chân trên vi điều khiển có thể lập trình để truyền nhận tín 
hiệu với các thiết bị bên ngoài hoặc thực hiện các chức năng giao tiếp 
khác.
 
GPIO hoạt động ở các chế độ sau:

### GPIO Output

Ở chế độ này, chân GPIO được sử dụng để xuất tín hiệu ra bên ngoài từ 
vi điều khiển.

Các loại Input Mode:

 - Floating hay High-impedance (thả nổi): Chân GPIO không có trạng thái điện áp xác định khi
  không có tín hiệu vào. 

    <p align="center">
        <img src="image.png" alt="alt text" width="200">
    </p>

    _Dễ bị nhiễu và tốn năng lượng, không nên sử dụng mode này!_


        
 - Pull-up: Chân GPIO ở mức logic cao khi không có tín hiệu đầu vào.
    <p align="center">
        <img src="image-1.png" alt="alt text" width="300">
    </p>    
 - Pull-down: Chân GPIO ở mức logic thấp khi không có tín hiệu đầu vào.

### GPIO Input

Ở chế độ này, chân GPIO được sử dụng để nhận tín hiệu từ bên ngoài 
vào vi điều khiển.

Các loại Output Mode:

 - Output Push-Pull: Tín hiệu xuất ra có thể là mức cao (1) hoặc mức
  thấp (0), và chân có thể đẩy dòng điện ra ngoài hoặc kéo dòng điện vào. Đây là chế độ thường dùng nhất.

 - Output Open-Drain: Chân chỉ có thể kéo xuống mức logic thấp (0) hoặc ở 
 trạng thái "thả nổi" (high impedance). Để tạo ra mức logic cao, thường 
 cần một điện trở kéo lên (pull-up). Chế độ này hữu ích khi giao tiếp 
 với các bus nhiều thiết bị như I2C.





### Alternate Function
### Analog
### EXTI (External Interrupt)



</p>
</details>

# LESSON 12: CAN(THEORY)
<details><summary>Details</summary>
<p>

</p>
</details>