# Projeto Final PI2 2023-2 Eng. Telecomunicações

> Desenvolvimento de sistema eletrônico para monitoramento e controle remoto para sala de aula com ESP32.

## Características:
- Sistema eletrônico para monitoramento e controle remoto para sala de aula;
- Monitorar e controlar ar condicionado (AC) e lâmpadas ligadas;
- Controle através de relê de potência;
- Utilizar hardware de controle cedido pelo professor: baseado no ESP32;
- Para controle do AC: sensor de temperatura;
- Para controle de lâmpadas ligadas: sensor LDR;
- Capacidade para monitorar até três salas/dispositivos;
- Possuir uma logomarca própria.

## Definições:
- Apresentação (front end) do controlador num dashboard (nuvem ou web embarcado: Raspbeery Pi,
por exemplo) ou num App para celular Android. Linguagem utilizada: livre;
- Back end: guardar informações tipo: senha acesso, endereço do dispositivo, etc. Linguagem
utilizada: livre.
- Programação do hardware: C/C++, Micropython, outra.
- Se deve utilizar o protocolo MQTT entre o hardware e o servidor;
- Realizar pesquisa da proposta do projeto.
  
## Desejáveis / desafios:
- Utilizar sistema de banco de dados ou semelhante para monitorar várias salas;
-  Agendamento de horários para habilitar o funcionamento do AC e das lâmpadas. Ex: ligar 07:30h e
desligar 11:35h; ligar 13:30 e desligar 17:35. Agendamento de segunda a sexta. Bloqueio para ligar
em feriados;
- Acesso / controle: administrador e usuário: funções a serem definidas;
