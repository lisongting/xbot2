Header header

# battery voltage range
float64  MAX_VOLTAGE = 29.4
float64  MIN_VOLTAGE = 24.0
# cloud platform rotate range
int8  MAX_CLOUD_DEGREE = 90
int8  MIN_CLOUD_DEGREE = -90

# camera rotate range
int8  MAX_CAMERA_DEGREE = 45
int8  MIN_CAMERA_DEGREE = -45

uint8  power_percent
uint8  height_percent
int8  cloud_degree
int8  camera_degree

# infred
float64 INFRED_MAX_RANGE = 0.5
float64 INFRED_MIN_RANGE = 0.2
float64 PLAT_HEIGHT = 0.1
bool is_hanged
bool front_left_hanged
bool front_center_hanged
bool front_right_hanged
bool rear_left_hanged
bool rear_center_hanged
bool rear_right_hanged
float64 dock_left_infred
float64 dock_right_infred

# echo
float64 ECHO_MAX_RANGE = 1.0
float64 ECHO_MIN_RANGE = 0.3
bool is_obstacle
float64 front_left_echo
float64 front_center_echo
float64 front_right_echo
float64 rear_left_echo
float64 rear_center_echo
float64 rear_right_echo
