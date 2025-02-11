import tkinter as tk
from tkinter import filedialog, messagebox, scrolledtext
import os
import threading

DEVICE_FILE = "/dev/pendrive_driver"  # Caminho do dispositivo

def mandar_pasta_para_driver():
    """Função para enviar o caminho selecionado ao driver."""
    path = filedialog.askdirectory(title="Selecione o diretório do pendrive")
    if path:
        try:
            with open(DEVICE_FILE, "w") as device:
                device.write(path)
            messagebox.showinfo("Sucesso", f"Path enviado ao driver:\n{path}")
        except Exception as e:
            messagebox.showerror("Erro", f"Erro ao enviar path: {e}")

def listar_arquivos_do_driver():
    """Função para solicitar listagem de arquivos ao driver."""
    try:
        with open(DEVICE_FILE, "w") as device:
            device.write("LIST_FILES")  # Comando específico para listar arquivos

        # Abrir nova janela para exibir os arquivos listados
        abrir_janela_listagem()

    except Exception as e:
        messagebox.showerror("Erro", f"Erro ao listar arquivos: {e}")

def abrir_janela_listagem():
    """Abre uma nova janela para exibir os arquivos listados."""
    # Criar nova janela
    janela_listagem = tk.Toplevel(root)
    janela_listagem.title("Arquivos Listados")
    janela_listagem.geometry("600x400")

    # Adicionar um widget Text para exibir os arquivos
    texto_listagem = scrolledtext.ScrolledText(janela_listagem, wrap=tk.WORD, width=70, height=20)
    texto_listagem.pack(padx=10, pady=10)

    # Variável de controle para a thread
    flag_parar = threading.Event()

    # Iniciar uma thread para ler o conteúdo do dmesg
    threading.Thread(target=ler_dmesg, args=(texto_listagem, flag_parar), daemon=True).start()

    # Função para fechar a janela e parar a thread
    def fechar_janela():
        flag_parar.set()  # Sinaliza para a thread parar
        janela_listagem.destroy()

    # Botão para fechar a janela
    btn_fechar = tk.Button(janela_listagem, text="Fechar", command=fechar_janela)
    btn_fechar.pack(pady=10)

    # Configurar o fechamento da janela para parar a thread
    janela_listagem.protocol("WM_DELETE_WINDOW", fechar_janela)

def ler_dmesg(texto_listagem, flag_parar):
    """Função para ler o conteúdo do dmesg e exibir os arquivos listados."""
    try:
        with open("/dev/kmsg", "r") as kmsg:
            while not flag_parar.is_set():  # Verifica a flag de parada
                linha = kmsg.readline()
                if "File:" in linha:
                    texto_listagem.insert(tk.END, linha)
                    texto_listagem.see(tk.END)  # Rolar para a última linha
    except Exception as e:
        messagebox.showerror("Erro", f"Erro ao ler arquivos listados: {e}")

def copiar_arquivo():
    """Função para copiar arquivos entre o pendrive e o disco local."""
    origem = filedialog.askopenfilename(title="Selecione o arquivo de origem")
    if origem:
        destino = filedialog.asksaveasfilename(title="Salvar como", defaultextension=".*")
        if destino:
            try:
                with open(DEVICE_FILE, "w") as device:
                    device.write(f"COPY:{origem}:{destino}")
                messagebox.showinfo("Sucesso", f"Arquivo copiado de:\n{origem}\npara:\n{destino}")
            except Exception as e:
                messagebox.showerror("Erro", f"Erro ao copiar arquivo: {e}")

def excluir_arquivo():
    """Função para excluir arquivos do pendrive."""
    arquivo = filedialog.askopenfilename(title="Selecione o arquivo para excluir")
    if arquivo:
        try:
            with open(DEVICE_FILE, "w") as device:
                device.write(f"DELETE:{arquivo}")
            messagebox.showinfo("Sucesso", f"Arquivo excluído:\n{arquivo}")
        except Exception as e:
            messagebox.showerror("Erro", f"Erro ao excluir arquivo: {e}")

# Interface gráfica principal
root = tk.Tk()
root.title("PRNDRIVE DRIVER")
root.geometry("720x450")
root.configure(background='white')

label = tk.Label(root, text="Selecione uma Ação para o Driver de Dispositivo", bg="white", font=("Arial", 20, 'bold'))
label.pack(pady=10)

btn_select_path = tk.Button(
    root,
    text="Selecionar Pendrive",
    font=("Arial", 22),
    command=mandar_pasta_para_driver,
    width=20,
    height=1,
    activebackground="Green",
    bg="yellow"
)
btn_select_path.pack(pady=10)

btn_list_files = tk.Button(
    root,
    text="Listar Arquivos",
    font=("Arial", 22),
    command=listar_arquivos_do_driver,
    width=20,
    height=1,
    activebackground="Green",
    bg="yellow"
)
btn_list_files.pack(pady=10)

btn_copy_file = tk.Button(
    root,
    text="Copiar Arquivo",
    font=("Arial", 22),
    command=copiar_arquivo,
    width=20,
    height=1,
    activebackground="Green",
    bg="yellow"
)
btn_copy_file.pack(pady=10)

btn_delete_file = tk.Button(
    root,
    text="Excluir Arquivo",
    font=("Arial", 22),
    command=excluir_arquivo,
    width=20,
    height=1,
    activebackground="Green",
    bg="yellow"
)
btn_delete_file.pack(pady=10)

btn_exit = tk.Button(
    root,
    text="Sair",
    fg="white",
    font=("Arial", 42),
    command=root.quit,
    width=5,
    height=1,
    activebackground="Green",
    bg="black"
)
btn_exit.pack(pady=30)

root.mainloop()
