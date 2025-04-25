import os
from reportlab.lib.pagesizes import letter
from reportlab.lib.styles import getSampleStyleSheet
from reportlab.platypus import SimpleDocTemplate, Paragraph, Spacer
from reportlab.lib.enums import TA_LEFT

def ler_arquivos_python(diretorio, ignorar_arquivos=None, ignorar_diretorios=None):
    codigos = []
    if ignorar_arquivos is None:
        ignorar_arquivos = []
    if ignorar_diretorios is None:
        ignorar_diretorios = []

    for raiz, dirs, arquivos in os.walk(diretorio):
        # Ignorar diretórios na lista de ignorar_diretorios
        dirs[:] = [d for d in dirs if d not in ignorar_diretorios and not d.startswith('.')]

        for arquivo in arquivos:
            # Ignorar arquivos na lista de ignorar_arquivos
            if (arquivo.endswith('.sh') or arquivo.endswith('.c') or arquivo.endswith('.h')) and arquivo not in ignorar_arquivos:
                caminho_completo = os.path.join(raiz, arquivo)
                try:
                    with open(caminho_completo, 'r', encoding='utf-8') as f:
                        codigos.append((caminho_completo, f.read()))
                except Exception as e:
                    print(f"Erro ao ler o arquivo {caminho_completo}: {e}")
    return codigos

def gerar_pdf(codigos, output_pdf):
    doc = SimpleDocTemplate(output_pdf, pagesize=letter)
    styles = getSampleStyleSheet()
    Story = []

    # Estilo personalizado para código
    code_style = styles["Code"]
    code_style.alignment = TA_LEFT
    code_style.fontName = "Courier"
    code_style.fontSize = 8
    code_style.leading = 10
    code_style.wordWrap = 'CJK'  # Permite quebra de linhas

    for caminho, codigo in codigos:
        # Adiciona o caminho do arquivo como título
        Story.append(Paragraph(f"<b>Arquivo:</b> {caminho}", styles['Heading2']))
        # Adiciona o código do arquivo como texto formatado
        Story.append(Paragraph(codigo.replace('\n', '<br/>'), code_style))
        # Adiciona um espaço entre os arquivos
        Story.append(Spacer(1, 12))

    try:
        doc.build(Story)
        print(f"PDF gerado com sucesso: {output_pdf}")
    except Exception as e:
        print(f"Erro ao gerar o PDF: {e}")

if __name__ == "__main__":
    # Diretório raiz do projeto
    diretorio_raiz = r"\Users\joao-\Desktop\so\producer_consumer_problem"
    
    
    # Nome do arquivo PDF de saída
    output_pdf = "codigos.pdf"

    # Lista de arquivos a serem ignorados
    ignorar_arquivos = ["api_teste.py", "cert.pem", "Guia de uso.pdf", "JSON_MODELS.txt", ".gitignore", "gerar_pdf.py", "key.pem", "instrucoes.txt", "pyvenv.cfg"]

    # Lista de diretórios a serem ignorados
    ignorar_diretorios = ["w64devkit", "unused", "bin", "executavelLinux", ".vscode", "Lib", "venv"]

    # Ler todos os arquivos Python, ignorando os especificados
    codigos = ler_arquivos_python(diretorio_raiz, ignorar_arquivos, ignorar_diretorios)

    # Gerar o PDF com os códigos
    gerar_pdf(codigos, output_pdf)