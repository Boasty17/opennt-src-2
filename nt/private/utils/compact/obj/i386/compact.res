L ê]@        .debug$S        ?   �               @ B.rsrc$01        �   �   l         @  @.rsrc$02        �  �              @  @    	     obj\i386\compact.res    Microsoft CVTRES 4.00     ê]             �   8  �    ê]          P  �    ê]          h  �    ê]       	  �       ê]       	  �     �	                        �       �       4   V S _ V E R S I O N _ I N F O     ���      �e   �e?                       n   S t r i n g F i l e I n f o   J   0 4 0 9 0 4 B 0   >   C o m p a n y N a m e     O p e n N T   P r o j e c t     T   F i l e D e s c r i p t i o n     F i l e   C o m p r e s s   U t i l i t y   *   F i l e V e r s i o n     4 . 0 0     0   I n t e r n a l N a m e   c o m p a c t   t (  L e g a l C o p y r i g h t   C o p y r i g h t   ( C )   M i c r o s o f t   C o r p .   1 9 8 1 - 1 9 9 6   @   O r i g i n a l F i l e n a m e   C O M P A C T . E X E   `    P r o d u c t N a m e     W i n d o w s   O p e n N T   O p e r a t i n g   S y s t e m   .   P r o d u c t V e r s i o n   4 . 0 0     D    V a r F i l e I n f o     $    T r a n s l a t i o n     	�         4         L         �  2   2   8     [OK]
     [ERR]
 P   
 Listing %1
 New files added to this directory will be compressed.

   T   
 Listing %1
 New files added to this directory will not be compressed.

   �   
Of %1 files within %2 directories
%3 are compressed and %4 are not compressed.
%5 total bytes of data are stored in %6 bytes.
The compression ratio is %7 to 1.

    @   
 Setting the directory %1 to compress new files %0

    $   
 Compressing files in %1

  $   
 Compressing files in %1

  �   
%1 files within %2 directories were compressed.
%3 total bytes of data are stored in %4 bytes.
The compression ratio is %5 to 1.
  D   
 Setting the directory %1 not to compress new files %0

    (   
 Uncompressing files in %1

    (   
 Uncompressing files in %1

    <   
%1 files within %2 directories were uncompressed.

    Out of memory.
       [Skipping %1]
    %1%0
     %1
    8   %1: The file system does not support compression.
    to 1 %0
   (   Invalid drive specification: %1
   x  Displays or alters the compression of files on NTFS partitions.

COMPACT [/C | /U] [/S[:dir]] [/A] [/I] [/F] [/Q] [filename [...]]

  /C        Compresses the specified files.  Directories will be marked
            so that files added afterward will be compressed.
  /U        Uncompresses the specified files.  Directories will be marked
            so that files added afterward will not be compressed.
  /S        Performs the specified operation on files in the given
            directory and all subdirectories.  Default "dir" is the
            current directory.
  /A        Displays files with the hidden or system attributes.  These
            files are omitted by default.
  /I        Continues performing the specified operation even after errors
            have occurred.  By default, COMPACT stops when an error is
            encountered.
  /F        Forces the compress operation on all specified files, even
            those which are already compressed.  Already-compressed files
            are skipped by default.
  /Q        Reports only the most essential information.
  filename  Specifies a pattern, file, or directory.

  Used without parameters, COMPACT displays the compression state of
  the current directory and any files it contains. You may use multiple
  filenames and wildcards.  You must put spaces between multiple
  parameters.
 .debug$S        .rsrc$02            