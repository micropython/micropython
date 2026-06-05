idf_component_register(
  SRCS "rp2040.c" "rp2040bl.c"
  INCLUDE_DIRS include
  PRIV_REQUIRES
    driver
)
