
1.make all 

2.To zip multiple files and folders: (ex. file1.txt, file2.txt, folder)
  To zip using HUffman coding->
  ./main zip 1 file1.txt file2.txt folder
      
   Creates an encodings file and a compressed file for each of the passed file, and for 
   all files in the folder 'folder' makes a new folder named 'folder_zipped' containing compressed files.
   
   Use '2' instead of '1' above to compress using Runlength coding.
   /main zip 2 file1.txt file2.txt folder
   
   Creates compressed files. No encodings file required hence not created.
3.To unzip
  Huffman
  ./main unzip 1 compressed_file.txt folderzipped  
  Runlength
  ./main unzip 2 compressed_file.txt folderzipped 

  For file, converts it to uncompressed file.
  For folder, creates a new folder with uncompressed files, named 'folderzipped_unzipped'

