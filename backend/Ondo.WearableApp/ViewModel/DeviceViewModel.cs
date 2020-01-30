using GalaSoft.MvvmLight.Command;
using Ondo.WearableApp.Model;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Threading.Tasks;

namespace Ondo.WearableApp
{
    public class DeviceViewModel : INotifyPropertyChanged
    {
        WebApiService webAPIService;
        public event PropertyChangedEventHandler PropertyChanged;
        private ObservableCollection<AirConDto> aircons;
        public RelayCommand<AirConDto> ChangeStateOfAirCon;

        public ObservableCollection<AirConDto> AirCons
        {
            get
            {
                return aircons;
            }
            set
            {
                aircons = value;
                RaisepropertyChanged("AirCons");
            }
        }

        public DeviceViewModel()
        {
            webAPIService = new WebApiService();
            aircons = new ObservableCollection<AirConDto>();
            ChangeStateOfAirCon = new RelayCommand<AirConDto>(ChangeState);
        }

        private async void ChangeState(AirConDto airConDto)
        {
            airConDto.DevicePower = !airConDto.DevicePower;
            await webAPIService.ChangeStateOfAirCon(airConDto);
        }

        public async Task GetData()
        {
            AirCons = await webAPIService.RefreshDataAsync();
        }
        void RaisepropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
                PropertyChanged.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}