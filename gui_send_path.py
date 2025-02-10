import tkinter as tk
from tkinter import filedialog, messagebox
import os

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
        messagebox.showinfo("Sucesso", "Comando LIST_FILES enviado ao driver!")
    except Exception as e:
        messagebox.showerror("Erro", f"Erro ao listar arquivos: {e}")

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

# Interface gráfica
root = tk.Tk()
root.title("UTFPR DRIVER")
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
