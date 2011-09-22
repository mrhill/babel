#!/usr/bin/perl
# Extract codepoint ranges of wide characters from EastAsianWidth.txt
# Codepoints marked as ambigious (A) are resolved as follows:
# - if member of a Latin codepage, assign normal width
# - else blend with widechar properties extracted from MS fonts:
#   if any win font says widechar, assign double width, otherwise normal width
#   (data extracted with unicode_iswide_winfont.cpp)

$key=1; # 1 = extract wide chars, 0 = extract normal width chars

$n_start=-23;
$o=-23;
$on=-23;
$c=0;
$min=-1;

@a_start = ();
@a_end = ();
@a_ct = ();

%isWide = (
 'A' => 2,
 'F' => 1,
 'H' => 0,
 'N' => 0,
 'Na' => 0,
 'W' => 1
);

@winfontfiles = ( 'unicode_iswide_msmincho.dat', 'unicode_iswide_gulimche.dat', 'unicode_iswide_nsimsun.dat' );
@winfonts = ();
foreach $filename (@winfontfiles) {
  open(FILE, $filename) or die "Can't open `$filename': $!";
  sysread FILE, $buffer, 8192;
  close FILE;
  @a = unpack('V2048', $buffer);
  push @winfonts, [@a];
}


$ver = <>; chop $ver;
($sec,$minu,$hour,$mday,$mon,$year,$wday,$yday,$isdst)=localtime(time);
printf "/* Generated from %s on %4d-%02d-%02d %02d:%02d:%02d */\n",$ver, $year+1900,$mon+1,$mday,$hour,$minu,$sec;

# parse input
while(<>) {
  if (substr($_, 0, 1) ne '#') {
    ($n,$v) = split(/[; ]/, $_);
    $n_end = index $n, '..';
    if ($n_end != -1) { $n_end = hex substr($n, $n_end+2); }
    $n=hex $n;
    $v=$isWide{$v};

    if ($v==2) {
      $v=0;
      if ($n >= 0x250) { # Assign all Latin chars to normal width
        for $a (@winfonts) {
          if ($a[$n>>5] & (1<<($n&31))) { $v=1; }
        }
      }
    }

    if (($v ne $o) || ($on+1 != $n)) {
      if (($c!=0) && ($o==$key)) {
        if ($min == -1) { $min=$n_start; }
        push @a_start, $n_start;
        push @a_end, $on;
        push @a_ct, $c;
      }
      $o=$v;
      $c=1;
      $n_start = $n;
    } else {
      $c++;
    }
    if ($n_end != -1) {
      $c+=$n_end-$n;
      $on=$n_end;
    } else {
      $on=$n;
    }
  }
}
if ($o==$key) {
  push @a_start, $n_start;
  push @a_end, $on;
  push @a_ct, $c;
}

@BMPIndex = (); for ($i=0; $i<256; $i++) { push @BMPIndex, 0xFF; }
@BMPMasks = ();
@BMPRanges = ();
$extct=0;

# Make sure, that a long range does not end on the same page
# as another range starts, split if needed
for($i=0; $i<$#a_ct; $i++) {
  $page = $a_start[$i]>>8;
  $pagend = $a_end[$i]>>8;
  if (($pageend != $page) && ($pageend == ($a_start[$i+1]>>8)))
  {
    print "xxx this code is not tested, please verify\n";
    splice(@a_start, $i+1, 0, $pageend<<8);
    splice(@a_end, $i+1, 0, $a_end[$i]);
    splice(@a_ct, $i+1, 0, $a_end[$i] & 0xFF);
    $a_end[$i] = ($pageend<<8) - 1;
    $a_ct[$i] -= $a_ct[$i+1];
  }
}

# Make sure, that if multiple ranges exist on the same 256 entry page,
# the last range does not exceed the page. If it does, split the last
# range into 2 ranges
$lastpage=-1;
for($i=0; $i<=$#a_ct; $i++) {
  $page = $a_start[$i]>>8;
  if (($page == $lastpage) && (($a_end[$i]>>8) != $page))
  {
    splice(@a_start, $i+1, 0, ($page+1)<<8);
    splice(@a_end, $i+1, 0, $a_end[$i]);
    splice(@a_ct, $i+1, 0, $a_ct[$i] - 256 + ($a_start[$i]&0xFF));
    $a_end[$i] = $a_start[$i] | 0xFF;
    $a_ct[$i] -= $a_ct[$i+1];
  }
  $lastpage = $page;
}

# human readable output
print "\n/* Wide char ranges:\n";
for($i=0; $i<=$#a_ct; $i++) {
  printf("0x%06X..0x%06X %d (%d)", $a_start[$i], $a_end[$i], $key, $a_ct[$i]);
  if ($i && ($a_start[$i]==($a_end[$i-1]+1))) { print " split\n" } else { print  "\n";}
}
print "*/\n";

# build data for gWideBMPIndex and gWideBMPData, and
# write if-return sequences for outside-BMP ranges
$lastpage=-1;
for($i=0; $i<=$#a_ct; $i++) {
  $start=$a_start[$i];
  $end=$a_end[$i];
  $ct=$a_ct[$i];

  if ($start >= 0x10000)
  {
    # Ranges outside BMP
    if (!$extct) { print "\n#ifdef bbCPWIDE_EXT\n"; }
    printf "if ((cp>=0x%06XUL)&&(cp<=0x%06XUL)) return 1;\n", $start, $end;
    $extct++;
  } else {
    # Ranges in BMP
    $page = $start>>8;
    if ($page == ($end>>8)) {
      # BMP range width mask
      if ($page != $lastpage) {
        # Create new 256 bitmask for this page
        $BMPIndex[$page] = $#BMPMasks+1;
        push @BMPMasks, 0,0,0,0,0,0,0,0;
      }
      # Set bits in mask for this range
      for($j=($start & 0xFF); $j<=($end & 0xFF); $j++) {
        $BMPMasks[$BMPIndex[$page] + ($j>>5)] |= 1 << ($j&31);
      }
    } else {
      # BMP large range width min max
      push @BMPRanges, $start | ($end<<16);
    }
    $lastpage = $page;
  }
}
print "#define bbCPWIDE_EXTCOUNT $extct\n";
print "#endif /* bbCPWIDE_EXT */\n";

# append large range data to index
$idx_largerange = $#BMPMasks+1;
for($i=0; $i<=$#BMPRanges; $i++) {
  $pageend   = ($BMPRanges[$i]>>24)&0xFF;
  for($j = ($BMPRanges[$i]>>8)&0xFF; $j<=$pageend; $j++) {
    $BMPIndex[$j] = $#BMPMasks+1;
  }
  push @BMPMasks, $BMPRanges[$i];
}

# write out
print "\n#ifdef bbCPWIDE_BMPINDEX\n";
printf("#define bbCPWIDE_MIN 0x%06XU\n", $min);
print "#define bbCPWIDE_IDX_LARGERANGE $idx_largerange\n";
for($i=0; $i<256; $i++) {
  printf '%3d', $BMPIndex[$i];
  if ($i<255) { print ','; }
  if (($i & 15)==15) { print "\n"; }
}
print "#endif /* bbCPWIDE_BMPINDEX */\n";

print "\n#ifdef bbCPWIDE_BMPDATA\n";
for($i=0; $i<=$#BMPMasks; $i++) {
  printf '0x%08XUL', $BMPMasks[$i]; 
  if ($i<$#BMPMasks) { print ','; }
  if (($i & 7)==7) { print "\n"; }
}
print "\n#endif /* bbCPWIDE_BMPDATA */\n";

