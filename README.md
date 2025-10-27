# Pendrive Driver - Sistemas Operacionais

## Descrição
Projeto desenvolvido como trabalho da disciplina **Sistemas Operacionais**.  
Trata-se de um **driver de pendrive** para Linux, acompanhado de uma **interface gráfica em Python** para interagir com ele.

Funcionalidades principais:
- Selecionar diretório do pendrive.
- Listar arquivos com logs do kernel.
- Copiar arquivos do pendrive para outro local.
- Excluir arquivos do pendrive.

## Estrutura do Projeto
```
.
├── gui_send_path.py      # Interface gráfica
├── pendrive_driver.c     # Driver de char device
├── Makefile              # Compila e gerencia o módulo
└── README.md             # Este arquivo
```

## Requisitos
- Linux
- Python 3.x
- Tkinter (`python3-tk`)
- Permissões de root para módulos do kernel

## Como Usar
1. Compilar o módulo:
```bash
make all
```
2. Inserir o módulo:
```bash
sudo make inserir
```
3. Configurar permissões:
```bash
sudo make setup
```
4. Executar a interface gráfica:
```bash
make interface
```
5. Ou executar tudo de uma vez:
```bash
make run
```
6. Para remover o módulo:
```bash
sudo make remover
```

## Autor
Trabalho desenvolvido para a disciplina **Sistemas Operacionais**  
Autor: *Seu Nome / RA*

## Licença
GPL
