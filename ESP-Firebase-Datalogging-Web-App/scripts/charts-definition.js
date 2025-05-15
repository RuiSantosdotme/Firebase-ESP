export function createTemperatureChart() {
  return Highcharts.chart('chart-temperature', {
    chart: {
      type: 'line',
      zoomType: 'x'
    },
    title: {
      text: 'Temperature'
    },
    xAxis: {
      type: 'datetime',
      title: {
        text: 'Time'
      }
    },
    yAxis: {
      title: {
        text: 'Temperature (°C)'
      }
    },
    series: [{
      name: 'Temperature',
      data: []
    }]
  });
}

export function createHumidityChart() {
  return Highcharts.chart('chart-humidity', {
    chart: {
      type: 'line',
      zoomType: 'x'
    },
    title: {
      text: 'Humidity'
    },
    xAxis: {
      type: 'datetime',
      title: {
        text: 'Time'
      }
    },
    yAxis: {
      title: {
        text: 'Humidity (%)'
      }
    },
    series: [{
      name: 'Humidity',
      data: []
    }]
  });
}

export function createPressureChart() {
  return Highcharts.chart('chart-pressure', {
    chart: {
      type: 'line',
      zoomType: 'x'
    },
    title: {
      text: 'Pressure'
    },
    xAxis: {
      type: 'datetime',
      title: {
        text: 'Time'
      }
    },
    yAxis: {
      title: {
        text: 'Pressure (hPa)'
      }
    },
    series: [{
      name: 'Pressure',
      data: []
    }]
  });
}
