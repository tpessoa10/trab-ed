# Usa uma imagem base com Linux e GCC instalado
FROM gcc:latest

# Define diretório de trabalho dentro do container
WORKDIR /app

# Copia os arquivos da pasta local para a pasta /app do container
COPY . /app

# Comando para compilar o código C++ automaticamente (opcional)
# Aqui você pode personalizar para seu projeto
CMD ["g++", "main.cpp", "-o", "main", "&&", "./main"]