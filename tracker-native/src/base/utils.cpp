//
// Created by liao xiangsen on 12/25/20.
//

#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

std::string hexDump(const uint8_t *bytes, size_t len) {
    std::ostringstream oss;
    char buff[17];
    size_t i = 0;

    oss << std::hex;

    // Process every byte in the data.
    for (i = 0; i < len; i++)
    {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0)
        {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
            {
                oss << "  " << buff << std::endl;
            }

            // Output the offset.
            oss << "  " << std::setw(4) << std::setfill('0') << static_cast<unsigned int>(i);
        }

        // Now the hex code for the specific character.
        oss << " " << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(bytes[i]);

        // And store a printable ASCII character for later.
        if ((bytes[i] < 0x20) || (bytes[i] > 0x7e))
        {
            buff[i % 16] = '.';
        }
        else
        {
            buff[i % 16] = bytes[i];
        }
        buff[(i % 16) + 1] = '\0';
    }

    oss << std::dec;

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0)
    {
        oss << "   ";
        i++;
    }

    // And print the final ASCII bit.
    oss << "  " << buff << std::endl;

    return oss.str();
}

