set(component_srcs "uart_driver.c")

idf_component_register(SRCS "${component_srcs}"
                        INCLUDE_DIRS "."
                        PRIV_REQUIRES "driver"
                        )