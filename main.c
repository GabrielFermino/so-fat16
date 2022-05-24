#include <stdlib.h>
#include <stdio.h>
/*Implemente um programa que faça a leitura de uma imagem FAT16 e apresente as seguintes infos:
a) Os arquivos e subdiretorios na raiz
b) Mostre o conteudo de um arquivo escolhido pelo usuario (utilizar o nome no formato 8.3 ou qualquer outra forma
c) Mostre o conteudo de um subdiretorio armazenado no diretorio raiz*/
typedef struct fat_BS{
  unsigned char         bootjmp[3];
  unsigned char         oem_name[8];
  unsigned short         bytes_per_sector;
  unsigned char        sectors_per_cluster;
  unsigned short        reserved_sector_count;
  unsigned char         table_count;
  unsigned short        root_entry_count;
  unsigned short        total_sectors_16;
  unsigned char         media_type;
  unsigned short        table_size_16;
  unsigned short        sectors_per_track;
  unsigned short        head_side_count;
  unsigned int          hidden_sector_count;
  unsigned int          total_sectors_32;

  //this will be cast to it's specific type once the driver actually knows what type of FAT this is.
  unsigned char        extended_section[54];

}__attribute__((packed)) fat_BS_t;

int main()
{
  FILE *fp;
  fat_BS_t  boot_record;
  unsigned int a, dirRaiz;

  fp= fopen("fat16_1sectorpercluster.img", "rb");
  fseek(fp, 0, SEEK_SET);
  fread(&boot_record, sizeof(fat_BS_t),1, fp);

  printf("Bytes per sector %hd \n", boot_record.bytes_per_sector);
  printf("Sector per cluster %x \n", boot_record.sectors_per_cluster);

  a = boot_record.reserved_sector_count + (boot_record.table_size_16 * boot_record.table_count);
  dirRaiz =  a * boot_record.bytes_per_sector;

  printf("a = %u\n", a);
  printf("diretorio Raiz = %u\n", dirRaiz);

  int pos_11, pos_ini;
  int conteudo_pos_ini, conteudo_pos_11;
  char nome[8], ext[3];
  
  pos_ini = dirRaiz;
  pos_11 = dirRaiz + 11;

  fseek(fp, pos_ini, SEEK_SET);
  fread(&conteudo_pos_ini, 1, 1, fp);

  while(conteudo_pos_ini != 0){

    fseek(fp, pos_ini, SEEK_SET);
    fread(&conteudo_pos_ini, 1, 1, fp);

     if(conteudo_pos_ini == 229){
      pos_11  += 32;
      pos_ini += 32;
    }
    else{
      fseek(fp, pos_11, SEEK_SET);
      fread(&conteudo_pos_11, 1, 1, fp);

      if(conteudo_pos_11 == 15){
        pos_11  += 32;
        pos_ini += 32;
      }
      else if(conteudo_pos_11 == 16){
        printf("DIRETORIO\n");
        fseek(fp, pos_ini, SEEK_SET);
        fread(&nome, sizeof(char), 8, fp);
        printf("Nome: %s \n", nome);
        printf("---------------------\n");
        pos_11  += 32;
        pos_ini += 32;
      }
      else if(conteudo_pos_11 == 32){
        printf("FILE\n");
        fseek(fp, pos_ini, SEEK_SET);
        fread(&nome, sizeof(char), 8, fp);
        printf("Nome: %s \n", nome);
        printf("---------------------\n");
        pos_11  += 32;
        pos_ini += 32;
      }
    }
  }
  return 0;
}