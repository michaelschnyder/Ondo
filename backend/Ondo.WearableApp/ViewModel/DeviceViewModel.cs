using GalaSoft.MvvmLight.Command;
using Ondo.Api.Devices;
using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Threading.Tasks;

namespace Ondo.WearableApp
{
    public class DeviceViewModel : INotifyPropertyChanged
    {
        WebApiService webAPIService;
        public event PropertyChangedEventHandler PropertyChanged;
        private ObservableCollection<DeviceDto> _devices;
        public  RelayCommand ChangeStateOfAirCon;

        public ObservableCollection<DeviceDto> Devices
        {
            get
            {
                return _devices;
            }
            set
            {
                _devices = value;
                RaisepropertyChanged("Devices");
            }
        }

        public DeviceViewModel()
        {
            webAPIService = new WebApiService();
            _devices = new ObservableCollection<DeviceDto>();
            ChangeStateOfAirCon = new RelayCommand(ChangeState);
        }

        private void ChangeState()
        {
            webAPIService.ChangeStateOfAirCon();
        }

        public async Task GetData()
        {
            Devices = await webAPIService.RefreshDataAsync();
        }
        void RaisepropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
                PropertyChanged.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}