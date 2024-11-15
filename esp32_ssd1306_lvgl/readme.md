# Projet setup and configuration

### create a new project
`idf.py create-project ssd1306_lvgl_demo`

### create folder "components"
`mkdir components`

### add ESP-IDF vscode configuration folder from command palette

### set target
`idf.py set-target esp32`

### run menuconfig
`idf.py menuconfig`

### add required components from ESP component registry
### https://components.espressif.com/

### required components:
#### lvgl 8.x
`idf.py add-dependency "lvgl/lvgl^8.4.0"`

#### esp_lvg_port
`idf.py add-dependency "espressif/esp_lvgl_port^2.4.2"`

### run menuconfig again
`idf.py menuconfig`

#### add new component to "components" folder 
`idf.py create-component ui`

#### modify component's CmakeLists.txt

#### design lvgl UI using EEZ Studio and copy the generated code to "components/ui" folder

#### reconfigure tast
`idf.py reconfigure`

#### build
`idf.py build`

#### flash 
`idf.py -p com4 flash monitor`