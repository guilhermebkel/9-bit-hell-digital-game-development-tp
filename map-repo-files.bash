# Gera um arquivo chamado all_files.txt com o conteúdo de todos os arquivos
output_file="all_files.txt"
> "$output_file"  # limpa o arquivo antes

# Percorre todos os arquivos, ignorando .git e o próprio output
find . \( -name "*.cpp" -o -name "*.h" \) \
  ! -path "./.git/*" \
  ! -name "$(basename "$output_file")" \
| while read -r file; do
  echo "# File Path: $file" >> "$output_file"
  echo '```' >> "$output_file"
  cat "$file" >> "$output_file"
  echo -e '\n```' >> "$output_file"
  echo >> "$output_file"
done
