/*
   Copyright (c) 2016, The CyanogenMod Project
   Copyright (c) 2019, The LineageOS Project
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <cstdlib>
#include <fstream>
#include <string.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <vector>

#include <android-base/logging.h>
#include <android-base/properties.h>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

#include "property_service.h"
#include "vendor_init.h"

using android::base::GetProperty;

void property_override(char const prop[], char const value[], bool add = true)
{
    auto pi = (prop_info *) __system_property_find(prop);
    if (pi != nullptr) {
        __system_property_update(pi, value, strlen(value));
    } else if (add) {
        __system_property_add(prop, strlen(prop), value, strlen(value));
    }
}

std::vector<std::string> ro_props_default_source_order = {
        "", "bootimage.", "odm.", "product.", "system.", "system_ext.", "vendor.",
};

void set_ro_build_prop(const std::string& prop, const std::string& value) {
    for (const auto& source : ro_props_default_source_order) {
        auto prop_name = "ro." + source + "build." + prop;
        if (source == "")
            property_override(prop_name.c_str(), value.c_str());
        else
            property_override(prop_name.c_str(), value.c_str(), false);
    }
};

void load_dalvik_properties()
{
	struct sysinfo sys;

	sysinfo(&sys);

	if (sys.totalram > 3072ull * 1024 * 1024)
	{
		// from - phone-xxhdpi-4096-dalvik-heap.mk
		property_override("dalvik.vm.heapstartsize", "8m");
		property_override("dalvik.vm.heaptargetutilization", "0.6");
		property_override("dalvik.vm.heapgrowthlimit", "256m");
		property_override("dalvik.vm.heapsize", "512m");
		property_override("dalvik.vm.heapmaxfree", "16m");
		property_override("dalvik.vm.heapminfree", "8m");
	}
	else
	{
                // from - phone-xhdpi-2048-dalvik-heap.mk
		property_override("dalvik.vm.heapstartsize", "8m");
		property_override("dalvik.vm.heaptargetutilization", "0.7");
		property_override("dalvik.vm.heapgrowthlimit", "192m");
		property_override("dalvik.vm.heapsize", "512m");
		property_override("dalvik.vm.heapmaxfree", "8m");
		property_override("dalvik.vm.heapminfree", "2m");
	}
}

void vendor_load_properties()
{
        std::string fingerprint;
        std::string description;

	// fingerprint
        fingerprint = "google/redfin/redfin:11/RQ3A.210705.001/7380771:user/release-keys";
        description = "redfin-user 11 RQ3A.210705.001 7380771 release-keys";

        set_ro_build_prop("fingerprint", fingerprint);
        property_override("ro.build.description", description.c_str());

        load_dalvik_properties();

	// Magisk Hide
	property_override("ro.boot.verifiedbootstate", "green");
	property_override("ro.boot.vbmeta.device_state", "locked");
	property_override("ro.boot.veritymode", "enforcing");
	property_override("ro.build.type", "user");
	property_override("ro.build.tags", "release-keys");
}
