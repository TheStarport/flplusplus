#pragma once

namespace logger {
    void writeline(const char *line);
    void writeformat(const char *fmt, ...);
    void patch_fdump();
}
