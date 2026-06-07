#include "Device/DevManager.hh"

int dev::Manager::Register(const dev::Handle &handle)
{
    if(!handle) {
        return -1;
    }
    if(Count_ >= G_kMaxDevices) {
        return -1;
    }
    for(std::size_t i= 0; i < Count_; ++i) {
        if(Entries_[i].devHandle_.GetName() == handle.GetName()) {
            return 0;
        }
    }
    Entries_[Count_++]= Entry { .devHandle_= handle, .pLegacy_= nullptr };
    return 0;
}

int dev::Manager::Register(IDevice *pDev)
{
    if(pDev == nullptr) {
        return -1;
    }
    if(Count_ >= G_kMaxDevices) {
        return -1;
    }
    for(std::size_t i= 0; i < Count_; ++i) {
        if(Entries_[i].pLegacy_ == pDev) {
            return 0;
        }
    }
    Entries_[Count_++]= Entry { .devHandle_= {}, .pLegacy_= pDev };
    return 0;
}

int dev::Manager::InitAll()
{
    for(std::size_t i= 0; i < Count_; ++i) {
        if(Entries_[i].pLegacy_ != nullptr) {
            if(Entries_[i].pLegacy_->Init() != 0) {
                return -1;
            }
        } else if(Entries_[i].devHandle_.Init() != 0) {
            return -1;
        }
    }
    return 0;
}

void dev::Manager::DeinitAll()
{
    for(std::size_t i= 0; i < Count_; ++i) {
        if(Entries_[i].pLegacy_) {
            Entries_[i].pLegacy_->Deinit();
        } else {
            Entries_[i].devHandle_.Deinit();
        }
    }
}

dev::Handle dev::Manager::FindHandle(std::string_view name) const
{
    for(std::size_t i= 0; i < Count_; ++i) {
        if(Entries_[i].pLegacy_ == nullptr && Entries_[i].devHandle_.GetName() == name) {
            return Entries_[i].devHandle_;
        }
    }
    return {};
}

IDevice *dev::Manager::Find(std::string_view name) const
{
    for(std::size_t i= 0; i < Count_; ++i) {
        if(Entries_[i].pLegacy_ != nullptr && Entries_[i].pLegacy_->GetName() == name) {
            return Entries_[i].pLegacy_;
        }
    }
    return nullptr;
}

std::size_t dev::Manager::GetCount() const noexcept
{
    return Count_;
}
