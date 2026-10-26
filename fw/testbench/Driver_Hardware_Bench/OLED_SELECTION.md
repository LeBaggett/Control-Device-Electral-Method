# Seleção do OLED

Em `Application/board/pinout.h`, o padrão agora é:

```c
#define OLED_SELECTED_DRIVER OLED_DRIVER_SSD1306
```

Para o outro controlador 128x64, troque somente por `OLED_DRIVER_SH1106`.
Depois faça **Project > Clean** e **Build Project** no CubeIDE e grave o novo firmware.
Não precisa regenerar o CubeMX para esta troca. O perfil é escolhido ao compilar.

SSD1306: coluna inicial 0; SH1106: deslocamento padrão de 2 colunas,
ajustável em `OLED_SH1106_COLUMN_OFFSET` nesse mesmo header.
Ambos são I2C 128x64. O tamanho físico sozinho não identifica o controlador.
O endereço `OLED_I2C_ADDRESS_7BIT` continua 0x3C (HAL recebe 0x78).
I2C1 permanece PB8/PB9 e os demais pinos não mudam.

O exemplo enviado para STM32F103 foi usado como referência dos comandos SSD1306;
foram preservados a aplicação STM32F407, os menus, fontes e splash existentes.
O SSD1306 usa o modo de páginas documentado 0x20/0x02 para o envio incremental.
Referência: https://www.olimex.com/Products/Modules/LCD/MOD-OLED-128x64/resources/SSD1306.pdf

Os testes de software não comprovam funcionamento físico. Confira primeiro no
projeto de bancada. Nesta versão manual, BOARD_POWER_CONTROL_ENABLED=1 permite comandos pelo usuário; a partida mantém relé desligado e SELECT em medição.
