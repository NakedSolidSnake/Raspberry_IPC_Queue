<p align="center">
  <img src="https://www.tutorialspoint.com/inter_process_communication/images/message_queue.jpg">
</p>

# _Queue System V_

## Tópicos
* [Introdução](#introdução)
* [Implementação](#implementação)
* [launch_processes](#launch_processes)
* [button_interface](#button_interface)
* [led_interface](#led_interface)
* [Compilando, Executando e Matando os processos](#compilando-executando-e-matando-os-processos)
* [Compilando](#compilando)
* [Clonando o projeto](#clonando-o-projeto)
* [Selecionando o modo](#selecionando-o-modo)
* [Modo PC](#modo-pc)
* [Modo RASPBERRY](#modo-raspberry)
* [Executando](#executando)
* [Interagindo com o exemplo](#interagindo-com-o-exemplo)
* [MODO PC](#modo-pc-1)
* [MODO RASPBERRY](#modo-raspberry-1)
* [Matando os processos](#matando-os-processos)
* [Conclusão](#conclusão)
* [Referência](#referência)

## Introdução
Existem três IPC's conhecidos como Queue, Semaphore e Shared Memory. Para esses IPC's existe dois padrões conhecidos como System V e POSIX. Neste artigo será apresentado uma rápida definição sobre o que são esses padrões e uma demonstração do uso de Queue System V. Cada um desses padrões tem sua forma de implementar o recurso que será visto conforme o desenvolvimento dos exemplos.

## System V
System V, também conhecido como AT&T System V, é uma das muitas versões do sistema operacional Unix. Foi originalmente desenvolvido pela AT&T e teve o primeiro lançamento em 1983. Com um total de 4 grandes versões de System V sendo as versões 1, 2, 3 e 4. System V Release 4(SVR4) é a versão que obteve mais sucesso e se tornou base para alguns recursos do UNIX, como o famoso initd que corresponde ao script de inicialização e de desligamento do sistema, o System V Interface Definition (SVID) é uma definição padrão de como o System V funciona. Para o link original clique [aqui](https://www.programmersought.com/article/93453872969/)

## POSIX
POSIX (Portable Operating System Interface for Computing Systems) é um conjunto de padrões desenvolvido pela IEEE e ISO/IEC. O padrão é basedo nas práticas e experiências existentes do UNIX, e descreve a interface de chamada de serviços do sistema operacional, que é usado para garantir que a aplicação pode ser portada e ser executada em uma gama de sistemas operacionais a nível de código fonte. Para o link original clique [aqui](https://www.programmersought.com/article/93453872969/)

## Queue System V
Queue System V permite que os processos troquem dados na forma de mensagens. Para se referir as mensagens é necessário um identificador. Comunicações feitas através de queues são orientadas a mensagens. As mensagens podem ser recuperadas no formato first-in, first-out ou pelo seu tipo e existem indenpendente de processos.

## Systemcalls

Para utilizar a API responsável por controlar a Queue é necessário criar uma estrutura com os campos type e um buffer para message.
```c
struct queue
{
  long type;
  char *message;
}
```

Retorna um identificador da Queue baseado em uma chave[key] e como vai ser configurada baseada em flags, que pode ser IPC_CREAT e IPC_EXCL.
```c
#include <sys/types.h> /* For portability */
#include <sys/msg.h>

int msgget(key_t key, int msgflg);
```

Para inserir mensagens na Queue é necessário preencher a estrutura com o tipo e o buffer. Para o envio é necessário um identificador da Queue passar a estrutura no argumento msgp, o tamanho da mensagem e a flag IPC_NOWAIT. No envio IPC_NOWAIT não faz tanta diferença e normalmente é usado 0.
```c
#include <sys/types.h> /* For portability */
#include <sys/msg.h>

int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
```
Para receber as mensagens da Queue é necessário um identificador, a estrutura com o tipo e o buffer, o tamanho da mensagem esperada, o tipo da mensagem e a flag IPC_NOWAIT. Neste caso a flag IPC_NOWAIT não aguarda a chegada de uma mensagem, para aguardar normalmente é usado o 0.
```c
#include <sys/types.h> /* For portability */
#include <sys/msg.h>

ssize_t msgrcv(int msqid, void *msgp, size_t maxmsgsz, long msgtyp, int msgflg);
```

Para remover, apagar ou configurar é usado msgctl onde para setar permissões ou consultar é nessário usar a estrutura apresentada na assinatura **struct msqid_ds**. As flags são IPC_STAT, IPC_SET e para remover a flag é IPC_RMID, nesse caso não é necessário passar o argumento da estrutura.
```c
#include <sys/types.h> /* For portability */
#include <sys/msg.h>

int msgctl(int msqid, int cmd, struct msqid_ds *buf);
```

## ipcs
A ferramenta ipcs é um utilitário para poder verificar o estado dos IPC's sendo eles: Queues, Semaphores e Shared Memory, o seu funcionanamento será demonstrado mais a seguir. Para mais informações execute:
```bash
$ man ipcs
```

## Implementação
Para facilitar a implementação a API da Queue foi abstraída para facilitar o uso.

### queue.h
É criado duas estruturas uma referente ao contexto da Queue que vai armazenar o identificador baseado em uma chave pré-determinada, bem como a definição da estrutura que vai ser usado para enviar e receber as mensagens

```c
#define BUFFER_SIZE 1024

typedef struct 
{
  long type;
  char buffer[BUFFER_SIZE];
} Queue_Data;

typedef struct
{
  int key;
  int id;
} Queue_t;
```

Neste ponto é criado uma API para abstrair a API da Queue System V, onde tem um função para iniciar, enviar, receber e destruir a Queue.
```c
bool Queue_Init(Queue_t *queue);

bool Queue_Send(Queue_t *queue, const Queue_Data *data, const int buffer_size);

bool Queue_Receive(Queue_t *queue, Queue_Data *data, const int buffer_size);

bool Queue_Destroy(Queue_t *queue);
```

### queue.c 
Queue_Init é responsável por criar a queue obtendo como resultado o identificador para uso e manipulação da queue
```c
bool Queue_Init(Queue_t *queue)
{
  bool status = false;
  
  do 
  {
    if (!queue)
      break;

    queue->id = msgget((key_t)queue->key, 0666 | IPC_CREAT);
    if (queue->id == -1)
      break;

    status = true;
  } while(false);
  
  return status;
}
```

Queue_Send é responsável por enviar as mensagens para a queue.
```c
bool Queue_Send(Queue_t *queue, const Queue_Data *data, const int buffer_size)
{
  bool status = true;
  if (msgsnd(queue->id, (void *)data, buffer_size, 0) == -1)
    status = false;
  return status;
}
```

Queue_Receive é responsável por ler as mensagens presentes na queue nessa API sempre vai aguardar uma mensagem, ou seja, é uma função blocante.
```c
bool Queue_Receive(Queue_t *queue, Queue_Data *data, const int buffer_size)
{
  bool status = true;
  if(msgrcv(queue->id, (void *)data, buffer_size, data->type, 0) == -1)
    status = false;

  return status;
}
```
Queue_Destroy é usada para remover a queue usando o identificador, nessa função é usado o comando IPC_RMID, após a execução a queue deixa de existir
```c
bool Queue_Destroy(Queue_t *queue)
{
  bool status = true;
  if( msgctl(queue->id, IPC_RMID, 0) == -1)
    status = false;
  
  return status;
}
```

Para demonstrar o uso desse IPC, será utilizado o modelo Produtor/Consumidor, onde o processo Produtor(_button_process_) vai escrever seu estado em uma mensagem, e inserir na queue, e o Consumidor(_led_process_) vai ler a mensagem da queue e aplicar ao seu estado interno. A aplicação é composta por três executáveis sendo eles:

* _launch_processes_ - é responsável por lançar os processos _button_process_ e _led_process_ através da combinação _fork_ e _exec_
* _button_interface_ - é responsável por ler o GPIO em modo de leitura da Raspberry Pi e escrever o estado em uma mensagem e inserir na queue
* _led_interface_ - é responsável por ler a mensagem da queue e aplicar em um GPIO configurado como saída

### *queue_context.h*
Para facilitar o nas interfaces foi criado um contexto que encapsula os tipos necessários para o seu uso já explicado anteriormente.
```c
typedef struct 
{
    Queue_t queue;
    Queue_Data data;
    int buffer_size;
} Queue_Context;
```

### *launch_processes.c*

No _main_ criamos duas variáveis para armazenar o PID do *button_process* e do *led_process*, e mais duas variáveis para armazenar o resultado caso o _exec_ venha a falhar.
```c
int pid_button, pid_led;
int button_status, led_status;
```

Em seguida criamos um processo clone, se processo clone for igual a 0, criamos um _array_ de *strings* com o nome do programa que será usado pelo _exec_, em caso o _exec_ retorne, o estado do retorno é capturado e será impresso no *stdout* e aborta a aplicação. Se o _exec_ for executado com sucesso o programa *button_process* será carregado. 
```c
pid_button = fork();

if(pid_button == 0)
{
    //start button process
    char *args[] = {"./button_process", NULL};
    button_status = execvp(args[0], args);
    printf("Error to start button process, status = %d\n", button_status);
    abort();
}   
```

O mesmo procedimento é repetido novamente, porém com a intenção de carregar o *led_process*.

```c
pid_led = fork();

if(pid_led == 0)
{
    //Start led process
    char *args[] = {"./led_process", NULL};
    led_status = execvp(args[0], args);
    printf("Error to start led process, status = %d\n", led_status);
    abort();
}
```

## *button_interface.h*
Neste header é criado um interface para quem for implementar precisa fornecer esses dois callbacks.
```c
typedef struct 
{
    bool (*Init)(void *object);
    bool (*Read)(void *object);
    
} Button_Interface;
```
Esta função é a função responsável por executar o loop da aplicação, onde fica aguardando um pressionamento do botão para enviar uma mensagem para a queue.
```c
bool Button_Run(void *object, Queue_Context *queue_context, Button_Interface *button);
```

## *button_interface.c*
Na função Button_Run inicializamos o botão e a queue
```c
if(button->Init(object) == false)
    return false;

if(Queue_Init(&queue_context->queue) == false)
    return false;
```
Neste ponto é aguardado um pressionamento de botão, altera-se o estado, formata em uma string e envia para a queue
```c
while(true)
{
    wait_press(object, button);

    state ^= 0x01;
    memset(queue_context->data.buffer, 0, queue_context->buffer_size);
    snprintf(queue_context->data.buffer, queue_context->buffer_size, "state = %d\n", state);
    Queue_Send(&queue_context->queue, &queue_context->data, queue_context->buffer_size);
}
```
Caso abandone o while a queue é destruída e retorna false.
```c
Queue_Destroy(&queue_context->queue);

return false;
```

## *led_interface.h*
Neste header é criado um interface para quem for implementar precisa fornecer esses dois callbacks.
```c
typedef struct 
{
    bool (*Init)(void *object);
    bool (*Set)(void *object, uint8_t state);
} LED_Interface;
```
Esta função é a função responsável por executar o loop da aplicação, onde fica aguardando uma mensagem para aplicar o estado ao LED.
```c
bool LED_Run(void *object, Queue_Context *queue_context, LED_Interface *led);
```

## *led_interface.c*
Aqui é inicializado o LED e a queue
```c
if (led->Init(object) == false)
        return false;

if(Queue_Init(&queue_context->queue) == false)
    return false;
```
Aqui a aplicação fica em loop aguardando uma mensagem para ser lida e assim extrair o estado que será aplicado ao LED
```c
while (true)
{
    if (Queue_Receive(&queue_context->queue, &queue_context->data, queue_context->buffer_size) == false)
    {
        continue;
    }

    sscanf(queue_context->data.buffer, "state = %d", &state_cur);
    memset(queue_context->data.buffer, 0, queue_context->buffer_size);

    if (state_cur != state_old)
    {

        state_old = state_cur;
        led->Set(object, state_cur);
    }
}
```
## Compilando, Executando e Matando os processos
Para compilar e testar o projeto é necessário instalar a biblioteca de [hardware](https://github.com/NakedSolidSnake/Raspberry_lib_hardware) necessária para resolver as dependências de configuração de GPIO da Raspberry Pi.

## Compilando
Para facilitar a execução do exemplo, o exemplo proposto foi criado baseado em uma interface, onde é possível selecionar se usará o hardware da Raspberry Pi 3, ou se a interação com o exemplo vai ser através de input feito por FIFO e o output visualizado através de LOG.

### Clonando o projeto
Pra obter uma cópia do projeto execute os comandos a seguir:

```bash
$ git clone https://github.com/NakedSolidSnake/Raspberry_IPC_Queue_SystemV
$ cd Raspberry_IPC_Queue_SystemV
$ mkdir build && cd build
```

### Selecionando o modo
Para selecionar o modo devemos passar para o cmake uma variável de ambiente chamada de ARCH, e pode-se passar os seguintes valores, PC ou RASPBERRY, para o caso de PC o exemplo terá sua interface preenchida com os sources presentes na pasta src/platform/pc, que permite a interação com o exemplo através de FIFO e LOG, caso seja RASPBERRY usará os GPIO's descritos no [artigo](https://github.com/NakedSolidSnake/Raspberry_lib_hardware#testando-a-instala%C3%A7%C3%A3o-e-as-conex%C3%B5es-de-hardware).

#### Modo PC
```bash
$ cmake -DARCH=PC ..
$ make
```

#### Modo RASPBERRY
```bash
$ cmake -DARCH=RASPBERRY ..
$ make
```

## Executando
Para executar a aplicação execute o processo _*launch_processes*_ para lançar os processos *button_process* e *led_process* que foram determinados de acordo com o modo selecionado.

```bash
$ cd bin
$ ./launch_processes
```

Uma vez executado podemos verificar se os processos estão rodando atráves do comando 
```bash
$ ps -ef | grep _process
```

O output 
```bash
cssouza  25162  2321  0 07:21 pts/1    00:00:00 ./button_process
cssouza  25163  2321  0 07:21 pts/1    00:00:00 ./led_process
```
## Interagindo com o exemplo
Dependendo do modo de compilação selecionado a interação com o exemplo acontece de forma diferente

### MODO PC
Para o modo PC, precisamos abrir um terminal e monitorar os LOG's
```bash
$ sudo tail -f /var/log/syslog | grep LED
```

Dessa forma o terminal irá apresentar somente os LOG's referente ao exemplo.

Para simular o botão, o processo em modo PC cria uma FIFO para permitir enviar comandos para a aplicação, dessa forma todas as vezes que for enviado o número 0 irá logar no terminal onde foi configurado para o monitoramento, segue o exemplo
```bash
$ echo '0' > /tmp/queue_file
```

Output dos LOG's quando enviado o comando algumas vezez
```bash
Jun 29 07:28:40 dell-cssouza LED QUEUE[25163]: LED Status: On
Jun 29 07:28:42 dell-cssouza LED QUEUE[25163]: LED Status: Off
Jun 29 07:28:43 dell-cssouza LED QUEUE[25163]: LED Status: On
Jun 29 07:28:43 dell-cssouza LED QUEUE[25163]: LED Status: Off
Jun 29 07:28:43 dell-cssouza LED QUEUE[25163]: LED Status: On
Jun 29 07:28:44 dell-cssouza LED QUEUE[25163]: LED Status: Off
```

### MODO RASPBERRY
Para o modo RASPBERRY a cada vez que o botão for pressionado irá alternar o estado do LED.

## ipcs funcionamento
Para inspecionar as queues presentes é necessário passar o argumento -q que representa queue, o comando fica dessa forma:
```bash
$ ipcs -q
```
O Output gerado na máquina onde o exemplo foi executado
```bash
------ Message Queues --------
key        msqid      owner      perms      used-bytes   messages    
0x000000c8 0          cssouza    666        0            0 
```

Para saber qual o processo que produz e qual consome é só passar o argumento -p, que desse modo será apresentado os PID's relacionados com essa queue. O comando fica dessa forma:
```bash
$ ipcs -q -p
```
O output apresenta os PID's de quem enviou a mensagem por último(lspid), e de quem recebeu a mensagem por último(lrpid), verificando os PID's com o comando ps feito anteriormente, é possível verificar que os PID's corresponde aos processos [25162, ./button_process] e [25163, ./led_process].
```bash
------ Message Queues PIDs --------
msqid      owner      lspid      lrpid     
0        cssouza     25162     25163
```

## Matando os processos
Para matar os processos criados execute o script kill_process.sh
```bash
$ cd bin
$ ./kill_process.sh
```

## Conclusão
A Queue é um IPC extremamente versátil principalmente por poder enviar mensagens para um processo específico, de forma a evitar que ocorra concorrência no acesso aos dados, já que a mensagem é destinada ao processo de interesse. A desvantagem no seu uso é para o caso de removê-la, pois a queue existe independente do processo, sendo assim caso o processo deixe de existir a queue permanecerá, devido a essa característica as vezes é necessário garantir que a queue seja destruída através de scripts, garantindo assim que não haverá mensagens antigas, que por outro lado pode ser até uma vantagem, caso o processo que esteja realizando a leitura venha cair poderá ser recuperado e processar as mensagens que foram enfileiradas.

## Referência
* [Link do projeto completo](https://github.com/NakedSolidSnake/Raspberry_IPC_Queue_SystemV)
* [Mark Mitchell, Jeffrey Oldham, and Alex Samuel - Advanced Linux Programming](https://www.amazon.com.br/Advanced-Linux-Programming-CodeSourcery-LLC/dp/0735710430)
* [fork, exec e daemon](https://github.com/NakedSolidSnake/Raspberry_fork_exec_daemon)
* [biblioteca hardware](https://github.com/NakedSolidSnake/Raspberry_lib_hardware)
* [artigo](https://www.programmersought.com/article/93453872969/)
