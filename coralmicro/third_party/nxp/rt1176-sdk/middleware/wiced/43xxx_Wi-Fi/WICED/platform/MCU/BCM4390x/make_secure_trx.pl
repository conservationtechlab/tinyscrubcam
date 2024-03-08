#!/usr/bin/perl
#
# Copyright 2021, Cypress Semiconductor Corporation or a subsidiary of 
 # Cypress Semiconductor Corporation. All Rights Reserved.
 # This software, including source code, documentation and related
 # materials ("Software"), is owned by Cypress Semiconductor Corporation
 # or one of its subsidiaries ("Cypress") and is protected by and subject to
 # worldwide patent protection (United States and foreign),
 # United States copyright laws and international treaty provisions.
 # Therefore, you may use this Software only as provided in the license
 # agreement accompanying the software package from which you
 # obtained this Software ("EULA").
 # If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 # non-transferable license to copy, modify, and compile the Software
 # source code solely for use in connection with Cypress's
 # integrated circuit products. Any reproduction, modification, translation,
 # compilation, or representation of this Software except as specified
 # above is prohibited without the express written permission of Cypress.
 #
 # Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 # EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 # WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 # reserves the right to make changes to the Software without notice. Cypress
 # does not assume any liability arising out of the application or use of the
 # Software or any product or circuit described in the Software. Cypress does
 # not authorize its products for use in any products where a malfunction or
 # failure of the Cypress product may reasonably be expected to result in
 # significant property damage, injury or death ("High Risk Product"). By
 # including Cypress's product in a High Risk Product, the manufacturer
 # of such system or application assumes all risk of such use and in doing
 # so agrees to indemnify Cypress against all liability.
#
# Adds an TRX (External Program Memory) header to
# encrypted and signed bin file, to allow it to be booted from serial flash

my ($trx, $bin, $out) = @ARGV;

# read and parse original trx file header
open TRX, '<', $trx or die $!;
binmode TRX;

my @trx_header = unpack('IIIIIIIII', <TRX>);
my ($magic, $total_size, $crc, $flag_ver, $size, $entry_point,
    $reserved0, $reserved1, $secure_delay) = @trx_header;

close TRX;

# get signed and encrypted file size
my $bin_size = -s $bin;

printf "\n[Original size in TRX header]\n";
printf "Total Size            : %d Bytes\n", $total_size;
printf "Size                  : %d Bytes\n", $size;
printf "Secure Option & Delay : 0x%08x\n", $secure_delay;

# update file size
$total_size = $bin_size + 36;
$size = $bin_size;
# default
# secure boot enabled, hmac-sha256 signed, aes-cbc128 encrypted, delay 400
# $secure_delay =    0x01900000;
# experimentally, default otp init delay, 20, is good enough.
$secure_delay =    0x00000000;

printf "\n[New aligned size in TRX header]\n";
printf "Total Size            : %d Bytes\n", $total_size;
printf "Size                  : %d Bytes\n", $size;
printf "Secure Option & Delay : 0x%08x\n", $secure_delay;

open BIN, '<:raw', $bin or die $!;
open OUT, '>:raw', $out or die $!;

seek OUT, 0, SEEK_SET;
print OUT pack('I', $magic);
print OUT pack('I', $total_size);
print OUT pack('I', $crc);
print OUT pack('I', $flag_ver);
print OUT pack('I', $size);
print OUT pack('I', $entry_point);
print OUT pack('I', $reserved0);
print OUT pack('I', $reserved1);
print OUT pack('I', $secure_delay);

my $buffer, $n;

while (($n = read BIN, $buffer, 16 * 1024) != 0) {
    print OUT $buffer;
}

close BIN;
close OUT;

print "\nDone.\n"
