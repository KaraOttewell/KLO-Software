"""CheckPdsZipIntegrityAndEmails.py - survey the VMS Free Software zip files.

Purpose
    A read-only health check of the downloads on the VMS Free Software page
    (index.html in this folder). For every .zip in the folder it:
      - tests the archive (the CRC of every member), and reports any bytes
        after the end of the archive - several of these zips carry VMS 512-byte
        block padding (trailing zeros), which is harmless and is deliberately
        left alone, because changing a byte would invalidate its signature;
      - counts the members and reports the newest member's date;
      - lists every email-like string found inside any member, so that you can
        see which addresses would be republished with the zips.
    Nothing is extracted to disk and no file is changed.

Usage (cmd)
    python -I CheckPdsZipIntegrityAndEmails.py [folder]

    With no folder, it checks the folder this script is in. For example, from
    the KLO-Software folder:

    python -I pds\\CheckPdsZipIntegrityAndEmails.py

Written 1 November 2026, when the page moved here from OneDrive WWW\\home\\pds.
"""
import re
import sys
import zipfile
from collections import defaultdict
from pathlib import Path

# Deliberately loose: it is better to list a near-miss than to miss an address.
EMAIL_PATTERN = re.compile(rb"[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}")


def describe_trailing_padding(archive_path):
    """Return (byte count, all zero?) for anything after the end-of-central-directory record.

    The record is 22 bytes plus an optional comment, whose length is stored at
    offset 20 within it. Returns None if the record cannot be found at all.
    """
    data = archive_path.read_bytes()
    end_record_offset = data.rfind(b"PK\x05\x06")
    if end_record_offset < 0:
        return None
    comment_length = int.from_bytes(data[end_record_offset + 20:end_record_offset + 22], "little")
    archive_end = end_record_offset + 22 + comment_length
    trailing = data[archive_end:]
    return len(trailing), all(byte == 0 for byte in trailing)


def main():
    folder = Path(sys.argv[1]) if len(sys.argv) > 1 else Path(__file__).resolve().parent
    addresses_by_zip = defaultdict(set)
    for archive_path in sorted(folder.glob("*.zip")):
        padding = describe_trailing_padding(archive_path)
        try:
            with zipfile.ZipFile(archive_path) as archive:
                bad_member = archive.testzip()
                members = archive.infolist()
                newest = max((member.date_time for member in members), default=None)
                for member in members:
                    if member.is_dir():
                        continue
                    contents = archive.read(member)
                    for match in EMAIL_PATTERN.findall(contents):
                        addresses_by_zip[archive_path.name].add(match.decode("ascii", "replace"))
                status = "OK" if bad_member is None else f"BAD member {bad_member}"
        except Exception as error:  # report it and carry on with the next archive
            status, members, newest = f"ERROR {error}", [], None
        padding_text = ""
        if padding and padding[0]:
            padding_text = f" (+{padding[0]} trailing {'zero ' if padding[1] else ''}bytes)"
        newest_text = "" if newest is None else "%04d-%02d-%02d" % newest[:3]
        print(f"{archive_path.name:40} {status:8} {len(members):4} members  newest {newest_text}{padding_text}")

    # Group by address, so each one is listed once with every zip it appears in
    print("\nEmail-like strings inside the archives:")
    zips_by_address = defaultdict(set)
    for zip_name, addresses in addresses_by_zip.items():
        for address in addresses:
            zips_by_address[address.lower()].add(zip_name)
    for address in sorted(zips_by_address):
        print(f"  {address:45} {', '.join(sorted(zips_by_address[address]))}")


if __name__ == "__main__":
    main()
