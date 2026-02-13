# Memoria - Arquitectura de Computadores

Este proyecto contiene la memoria del trabajo de Arquitectura de Computadores en formato LaTeX.

## Requisitos Previos

Este documento utiliza **XeLaTeX** como motor de compilación y requiere:
- XeLaTeX (incluido en distribuciones TeX completas)
- Biber (para la gestión de bibliografía)
- Fuente: Myriad Pro (instalada en el sistema)

---

## Instalación por Sistema Operativo

### 🍎 macOS

#### Opción 1: MacTeX (Recomendado)
```bash
# Descargar e instalar MacTeX desde:
# https://www.tug.org/mactex/

# O usando Homebrew:
brew install --cask mactex
```

#### Opción 2: BasicTeX (versión ligera)
```bash
brew install --cask basictex

# Instalar paquetes necesarios
sudo tlmgr update --self
sudo tlmgr install biblatex biber fontspec babel csquotes babel-spanish \
    geometry fancyhdr titlesec xcolor hyperref amsmath float caption \
    subcaption microtype eso-pic placeins booktabs tabularx array colortbl
```

#### Editor Recomendado
- **Visual Studio Code** con extensión LaTeX Workshop
- **TeXShop** (incluido con MacTeX)
- **Overleaf** (online)

---

### 🪟 Windows

#### Opción 1: MiKTeX (Recomendado)
```powershell
# Descargar e instalar MiKTeX desde:
# https://miktex.org/download

# MiKTeX instalará automáticamente los paquetes faltantes al compilar
```

#### Opción 2: TeX Live
```powershell
# Descargar e instalar TeX Live desde:
# https://www.tug.org/texlive/windows.html
```

#### Editor Recomendado
- **Visual Studio Code** con extensión LaTeX Workshop
- **TeXstudio** (https://www.texstudio.org/)
- **TeXworks** (incluido con MiKTeX)

---

### 🐧 Linux

#### Ubuntu/Debian
```bash
# Instalación completa
sudo apt update
sudo apt install texlive-xetex texlive-bibtex-extra biber \
    texlive-lang-spanish texlive-fonts-extra texlive-science

# Verificar instalación
xelatex --version
biber --version
```

#### Fedora/RHEL
```bash
sudo dnf install texlive-scheme-full biber
```

#### Arch Linux
```bash
sudo pacman -S texlive-most texlive-langextra biber
```

#### Editor Recomendado
- **Visual Studio Code** con extensión LaTeX Workshop
- **TeXstudio**
- **Kile**

---

## Instalación de la Fuente Myriad Pro

### macOS
1. Descargar Myriad Pro (si no está instalada)
2. Hacer doble clic en los archivos `.otf` o `.ttf`
3. Clic en "Instalar fuente"
4. Reiniciar el editor LaTeX

### Windows
1. Descargar Myriad Pro
2. Clic derecho en los archivos de fuente → "Instalar"
3. Reiniciar el editor LaTeX

### Linux
```bash
# Copiar fuentes a directorio local
mkdir -p ~/.fonts
cp MyriadPro*.otf ~/.fonts/

# Actualizar caché de fuentes
fc-cache -f -v

# Verificar instalación
fc-list | grep "Myriad"
```

**Alternativa**: Si no tienes Myriad Pro, puedes cambiar la fuente en [memoria.tex](memoria.tex) línea 8:
```latex
% \setmainfont{Myriad Pro}    % Fuente original
\setmainfont{Arial}           % Alternativa en Windows
% \setmainfont{Liberation Sans} % Alternativa en Linux
```

---

## Compilación del Documento

### Desde la Terminal

```bash
# Compilar documento (ejecutar en orden)
xelatex memoria.tex
biber memoria
xelatex memoria.tex
xelatex memoria.tex

# O usar latexmk para compilación automática
latexmk -xelatex -synctex=1 -interaction=nonstopmode memoria.tex
```

### Desde Visual Studio Code

1. **Instalar extensión**: LaTeX Workshop (James-Yu.latex-workshop)
2. **Configurar** (opcional) el archivo `.vscode/settings.json`:
```json
{
    "latex-workshop.latex.recipes": [
        {
            "name": "XeLaTeX + Biber",
            "tools": [
                "xelatex",
                "biber",
                "xelatex",
                "xelatex"
            ]
        }
    ],
    "latex-workshop.latex.tools": [
        {
            "name": "xelatex",
            "command": "xelatex",
            "args": [
                "-synctex=1",
                "-interaction=nonstopmode",
                "-file-line-error",
                "%DOC%"
            ]
        },
        {
            "name": "biber",
            "command": "biber",
            "args": [
                "%DOCFILE%"
            ]
        }
    ],
    "latex-workshop.view.pdf.viewer": "tab"
}
```
3. **Compilar**: Abrir `memoria.tex` y presionar `Ctrl+Alt+B` (o `Cmd+Alt+B` en macOS)
4. **Ver PDF**: Presionar `Ctrl+Alt+V` (o `Cmd+Alt+V` en macOS)

### Desde otros editores

- **TeXShop**: Seleccionar XeLaTeX en el menú desplegable y presionar "Componer"
- **TeXstudio**: Configurar "Compilar" → "XeLaTeX" y presionar F5
- **TeXworks**: Seleccionar XeLaTeX en el menú y presionar el botón verde

---

## Estructura del Proyecto

```
.
├── memoria.tex          # Documento principal
├── referencias.bib      # Base de datos bibliográfica
├── memoria.bbl          # Bibliografía compilada (generada automáticamente)
├── images/              # Directorio de imágenes
└── README.md           # Este archivo
```

---

## Solución de Problemas Comunes

### Error: "Font 'Myriad Pro' not found"
- **Solución**: Instalar la fuente Myriad Pro o cambiar a una fuente alternativa (ver sección de fuentes)

### Error: "biber command not found"
- **macOS/Linux**: `sudo tlmgr install biber`
- **Windows (MiKTeX)**: Usar MiKTeX Package Manager para instalar biber

### Bibliografía no aparece
1. Asegúrate de compilar en este orden: `xelatex → biber → xelatex → xelatex`
2. Verifica que [referencias.bib](referencias.bib) contiene las referencias citadas
3. Usa `\cite{clave}` para citar referencias en el texto

### Imágenes no aparecen
- Verifica que las imágenes están en el directorio `images/`
- Usa rutas relativas: `\includegraphics{images/nombre.png}`

### Errores de compilación
- Revisa el log de errores (`.log` file)
- Asegúrate de que todos los paquetes están instalados
- Verifica que no hay caracteres especiales sin escapar

---