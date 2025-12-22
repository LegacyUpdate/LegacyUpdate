#pragma once

void BecomeDPIAware();
void IsolationAwareStart(ULONG_PTR *cookie);
void IsolationAwareEnd(ULONG_PTR *cookie);
