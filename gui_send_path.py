import tkinter as tk  
from tkinter import filedialog, messagebox, scrolledtext  
import os  
import threading  

DEVICE_FILE = "/dev/pendrive_driver"  # Caminho do dispositivo do driver

def mandar_pasta_para_driver():
    """Função para enviar o caminho selecionado ao driver."""
    path = filedialog.askdirectory(title="Selecione o diretório do pendrive")  # Abre uma janela para selecionar o diretório
    if path:
        try:
            with open(DEVICE_FILE, "w") as device:
                device.write(path)  # Envia o caminho para o driver
            messagebox.showinfo("Sucesso", f"Path enviado ao driver:\n{path}")  # Exibe mensagem de sucesso
        except Exception as e:
            messagebox.showerror("Erro", f"Erro ao enviar path: {e}")  # Exibe mensagem de erro

def listar_arquivos_do_driver():
    """Função para solicitar listagem de arquivos ao driver."""
    try:
        with open(DEVICE_FILE, "w") as device:
            device.write("LIST_FILES")  # Envia o comando para listar arquivos

        # Abrir nova janela para exibir os arquivos listados
        abrir_janela_listagem()

    except Exception as e:
        messagebox.showerror("Erro", f"Erro ao listar arquivos: {e}")  # Exibe mensagem de erro

def abrir_janela_listagem():
    """Abre uma nova janela para exibir os arquivos listados."""
    # Criar nova janela
    janela_listagem = tk.Toplevel(root)  # Janela secundária
    janela_listagem.title("Arquivos Listados")  # Título da janela
    janela_listagem.geometry("600x400")  # Tamanho da janela

    # Adicionar um widget Text para exibir os arquivos
    texto_listagem = scrolledtext.ScrolledText(janela_listagem, wrap=tk.WORD, width=70, height=20)  # Área de texto com barra de rolagem
    texto_listagem.pack(padx=10, pady=10)  # Posiciona o widget na janela

    # Variável de controle para a thread
    flag_parar = threading.Event()  # Evento para controlar a execução da thread

    # Iniciar uma thread para ler o conteúdo do dmesg
    threading.Thread(target=ler_dmesg, args=(texto_listagem, flag_parar), daemon=True).start()  # Thread para ler o dmesg

    # Função para fechar a janela e parar a thread
    def fechar_janela():
        flag_parar.set()  # Sinaliza para a thread parar
        janela_listagem.destroy()  # Fecha a janela

    # Botão para fechar a janela
    btn_fechar = tk.Button(janela_listagem, text="Fechar", command=fechar_janela)  # Botão de fechar
    btn_fechar.pack(pady=10)  # Posiciona o botão na janela

    # Configurar o fechamento da janela para parar a thread
    janela_listagem.protocol("WM_DELETE_WINDOW", fechar_janela)  # Define o comportamento ao fechar a janela

def ler_dmesg(texto_listagem, flag_parar):
    """Função para ler o conteúdo do dmesg e exibir os arquivos listados."""
    try:
        with open("/dev/kmsg", "r") as kmsg:  # Abre o arquivo de log do kernel
            while not flag_parar.is_set():  # Verifica a flag de parada
                linha = kmsg.readline()  # Lê uma linha do log
                if "File:" in linha:  # Verifica se a linha contém informações de arquivos
                    texto_listagem.insert(tk.END, linha)  # Insere a linha no widget de texto
                    texto_listagem.see(tk.END)  # Rolar para a última linha
    except Exception as e:
        messagebox.showerror("Erro", f"Erro ao ler arquivos listados: {e}")  # Exibe mensagem de erro

def copiar_arquivo():
    """Função para copiar arquivos entre o pendrive e o disco local."""
    origem = filedialog.askopenfilename(title="Selecione o arquivo de origem")  # Abre uma janela para selecionar o arquivo de origem
    if origem:
        destino = filedialog.asksaveasfilename(title="Salvar como", defaultextension=".*")  # Abre uma janela para selecionar o destino
        if destino:
            try:
                with open(DEVICE_FILE, "w") as device:
                    device.write(f"COPY:{origem}:{destino}")  # Envia o comando para copiar o arquivo
                messagebox.showinfo("Sucesso", f"Arquivo copiado de:\n{origem}\npara:\n{destino}")  # Exibe mensagem de sucesso
            except Exception as e:
                messagebox.showerror("Erro", f"Erro ao copiar arquivo: {e}")  # Exibe mensagem de erro

def excluir_arquivo():
    """Função para excluir arquivos do pendrive."""
    arquivo = filedialog.askopenfilename(title="Selecione o arquivo para excluir")  # Abre uma janela para selecionar o arquivo a ser excluído
    if arquivo:
        try:
            with open(DEVICE_FILE, "w") as device:
                device.write(f"DELETE:{arquivo}")  # Envia o comando para excluir o arquivo
            messagebox.showinfo("Sucesso", f"Arquivo excluído:\n{arquivo}")  # Exibe mensagem de sucesso
        except Exception as e:
            messagebox.showerror("Erro", f"Erro ao excluir arquivo: {e}")  # Exibe mensagem de erro

# Interface gráfica principal
root = tk.Tk()  # Cria a janela principal
root.title("UTFPR DRIVER")  # Título da janela
root.geometry("720x450")  # Tamanho da janela
root.configure(background='white')  # Cor de fundo da janela

label = tk.Label(root, text="Selecione uma Ação para o Driver de Dispositivo", bg="white", font=("Arial", 20, 'bold'))  # Rótulo da interface
label.pack(pady=10)  # Posiciona o rótulo na janela

# Botão para selecionar o pendrive
btn_select_path = tk.Button(
    root,
    text="Selecionar Pendrive",
    font=("Arial", 22),
    command=mandar_pasta_para_driver,  # Ação ao clicar no botão
    width=20,
    height=1,
    activebackground="Green",
    bg="yellow"
)
btn_select_path.pack(pady=10)  # Posiciona o botão na janela

# Botão para listar arquivos
btn_list_files = tk.Button(
    root,
    text="Listar Arquivos",
    font=("Arial", 22),
    command=listar_arquivos_do_driver,  # Ação ao clicar no botão
    width=20,
    height=1,
    activebackground="Green",
    bg="yellow"
)
btn_list_files.pack(pady=10)  # Posiciona o botão na janela

# Botão para copiar arquivos
btn_copy_file = tk.Button(
    root,
    text="Copiar Arquivo",
    font=("Arial", 22),
    command=copiar_arquivo,  # Ação ao clicar no botão
    width=20,
    height=1,
    activebackground="Green",
    bg="yellow"
)
btn_copy_file.pack(pady=10)  # Posiciona o botão na janela

# Botão para excluir arquivos
btn_delete_file = tk.Button(
    root,
    text="Excluir Arquivo",
    font=("Arial", 22),
    command=excluir_arquivo,  # Ação ao clicar no botão
    width=20,
    height=1,
    activebackground="Green",
    bg="yellow"
)
btn_delete_file.pack(pady=10)  # Posiciona o botão na janela

# Botão para sair
btn_exit = tk.Button(
    root,
    text="Sair",
    fg="white",
    font=("Arial", 42),
    command=root.quit,  # Ação ao clicar no botão (fechar a aplicação)
    width=5,
    height=1,
    activebackground="Green",
    bg="black"
)
btn_exit.pack(pady=30)  # Posiciona o botão na janela

root.mainloop()  # Inicia o loop principal da interface gráfica
