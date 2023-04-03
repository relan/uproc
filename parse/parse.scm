;
; Copyright (C) 2013-2016  Andrew Nayenko
;
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>.
;

(require-extension srfi-151)

(define nil '())

(define (test-feature dependency feature)
  (cond
    ((equal? dependency nil) nil)
    ((equal? dependency #f) #f)
    ((equal? feature nil) nil)
    (else (not (= feature 0)))))

(define (print-feature name feature)
  (print " - " name ": "
    (cond
      ((equal? feature #t) "yes")
      ((equal? feature #f) "no")
      (else "?"))))

(define (print-section name)
  (print "\n" name " features:\n"))

(define eax 0)
(define ebx 1)
(define ecx 2)
(define edx 3)

(define (cpuid dependency leaf register bit)
  (test-feature
    dependency
    (if (assoc leaf cpuid-map)
      (bitwise-and
        (vector-ref (cadr (assoc leaf cpuid-map)) register)
        (arithmetic-shift 1 bit))
      0))) ; Feature is not supported if appropriate leaf does not exist

(define (msr dependency id bit)
  (test-feature
    dependency
    (if (assoc id msr-map)
      (bitwise-and
        (cadr (assoc id msr-map))
        (arithmetic-shift 1 bit))
      nil))) ; Feature status unknown if appropriate MSR is missing

(define cpuid-map (read))
(define msr-map (read))

(print-section "Generic")

(print-feature "Physical Address Extension"
  (cpuid #t #x1 edx 6))
(print-feature "Physical Attributes Table"
  (cpuid #t #x1 edx 16))
(print-feature "MONITOR/MWAIT"
  (cpuid #t #x1 ecx 3))
(print-feature "x2APIC"
  (define x2apic (cpuid #t #x1 ecx 21)))
(print-feature "x2APIC is active"
  (msr x2apic "IA32_APIC_BASE" 10))
(print-feature "XSAVE instruction"
  (define xsave (cpuid #t #x1 ecx 26)))
(print-feature "XSAVE is active"
  (cpuid xsave #x1 ecx 27))
(print-feature "Long Mode"
  (define lm (cpuid #t #x80000001 edx 29)))
(print-feature "Long Mode is active"
  (msr lm "IA32_EFER" 10))
(print-feature "No-Execute Page Protection"
  (define nx (cpuid #t #x80000001 edx 20)))
(print-feature "No-Execute Page Protection is active"
  (msr nx "IA32_EFER" 11))
(print-feature "Process-Context Identifiers"
  (cpuid #t #x1 ecx 17))
(print-feature "RDRAND instruction"
  (cpuid #t #x1 ecx 30))
(print-feature "Bit Manipulation Instructions"
  (cpuid #t #x7 ebx 3))
(print-feature "Bit Manipulation Instructions 2"
  (cpuid #t #x7 ebx 8))
(print-feature "Supervisor-Mode Execution Prevention"
  (cpuid #t #x7 ebx 7))
(print-feature "Supervisor-Mode Access Prevention"
  (cpuid #t #x7 ebx 20))
(print-feature "Hardware Lock Elision"
  (cpuid #t #x7 ebx 4))
(print-feature "Restricted Transactional Memory"
  (cpuid #t #x7 ebx 11))
(print-feature "Feature Control Locked"
  (msr #t "IA32_FEATURE_CONTROL" 0))

(print-section "SIMD")

(print-feature "MultiMedia eXtensions"
  (cpuid #t #x1 edx 23))
(print-feature "Steaming SIMD Extensions"
  (cpuid #t #x1 edx 25))
(print-feature "Steaming SIMD Extensions 2"
  (cpuid #t #x1 edx 26))
(print-feature "Steaming SIMD Extensions 3"
  (cpuid #t #x1 ecx 0))
(print-feature "Supplemental Steaming SIMD Extensions 3"
  (cpuid #t #x1 ecx 9))
(print-feature "Steaming SIMD Extensions 4.1"
  (cpuid #t #x1 ecx 19))
(print-feature "Steaming SIMD Extensions 4.2"
  (cpuid #t #x1 ecx 20))
(print-feature "AESNI instructions"
  (cpuid #t #x1 ecx 25))
(print-feature "Advanced Vector Extensions"
  (cpuid #t #x1 ecx 28))
(print-feature "Advanced Vector Extensions 2"
  (cpuid #t #x7 ebx 5))
(print-feature "Fused Multiply-Add"
  (cpuid #t #x1 ecx 12))

(print-section "Intel VT-x")

(print-feature "Virtual Machine eXtension"
  (define vmx (cpuid #t #x1 ecx 5)))
(print-feature "VMX outside SMX"
  (msr vmx "IA32_FEATURE_CONTROL" 2))
(print-feature "MSR Bitmaps"
  (msr vmx "IA32_VMX_PROCBASED_CTLS" 60))
(print-feature "TPR Shadow"
  (msr vmx "IA32_VMX_PROCBASED_CTLS" 53))
(print-feature "Secondary Controls"
  (define secondary_ctls (msr vmx "IA32_VMX_PROCBASED_CTLS" 63)))
(print-feature "Virtual APIC"
  (msr secondary_ctls "IA32_VMX_PROCBASED_CTLS2" 32))
(print-feature "Preemption Timer"
  (msr vmx "IA32_VMX_PINBASED_CTLS" 38))
(print-feature "Posted Interrupts"
  (msr vmx "IA32_VMX_PINBASED_CTLS" 39))
(print-feature "Extended Page Tables"
  (msr secondary_ctls "IA32_VMX_PROCBASED_CTLS2" 33))
(print-feature "Descriptor-table exiting"
  (msr secondary_ctls "IA32_VMX_PROCBASED_CTLS2" 34))
(print-feature "Virtual Processor Identifiers"
  (msr secondary_ctls "IA32_VMX_PROCBASED_CTLS2" 37))
(print-feature "Unrestricted Guest"
  (msr secondary_ctls "IA32_VMX_PROCBASED_CTLS2" 39))
(print-feature "PAUSE-Loop Exiting"
  (msr secondary_ctls "IA32_VMX_PROCBASED_CTLS2" 42))
(print-feature "VMCS Shadowing"
  (msr secondary_ctls "IA32_VMX_PROCBASED_CTLS2" 46))
(print-feature "EPT-Violation #VE"
  (msr secondary_ctls "IA32_VMX_PROCBASED_CTLS2" 50))

(print-section "AMD SVM")

(print-feature "Secure Virtual Machine"
  (define svm (cpuid #t #x80000001 ecx 2)))
(print-feature "Nested Paging"
  (cpuid svm #x8000000a edx 0))
(print-feature "Next RIP Save"
  (cpuid svm #x8000000a edx 3))
(print-feature "VMCB Clean Bits"
  (cpuid svm #x8000000a edx 5))
(print-feature "Flush By ASID"
  (cpuid svm #x8000000a edx 6))
(print-feature "Decode Assists"
  (cpuid svm #x8000000a edx 7))
(print-feature "Pause Filter"
  (cpuid svm #x8000000a edx 10))
(print-feature "Pause Filter Threshold"
  (cpuid svm #x8000000a edx 12))
(print-feature "Advanced Virtual Interrupt Controller"
  (cpuid svm #x8000000a edx 13))

(print)
