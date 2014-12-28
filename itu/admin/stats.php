<div class="col-xs-12 col-sm-6 col-md-8">
  <h2>Monthly visits</h2>
  <div class="panel panel-default">
    <div class="panel-body">
      <canvas id="monthly" height="40" width="100" style="padding-right: 30px;"></canvas>
      <script>
        var monthlyData = {
            labels: ["January", "February", "March", "April", "May", "June", "July"],
            datasets: [
                {
                    label: "",
                    fillColor: "rgba(220,220,220,0.2)",
                    strokeColor: "rgba(220,220,220,1)",
                    pointColor: "rgba(220,220,220,1)",
                    pointStrokeColor: "#fff",
                    pointHighlightFill: "#fff",
                    pointHighlightStroke: "rgba(220,220,220,1)",
                    data: [1265, 1259, 1280, 2281, 2156, 2295, 1322]
                }
            ]

        }
      </script>
    </div>
  </div>
  <h2>Users platform & Browsers</h2>
  <div class="panel panel-default">
    <div class="panel-body">
      <div class="canvas-holder-half">
        <canvas id="platform" height="40" width="50" style="padding-right: 30px;"></canvas>
        <div id="platformLegend"></div>
        <script>
          var platformData = [
              {
                  value: 300,
                  color:"#F7464A",
                  highlight: "#FF5A5E",
                  label: "Windows 7"
              },
              {
                  value: 400,
                  color:"#F7464A",
                  highlight: "#FF5A5E",
                  label: "Windows 8"
              },
              {
                  value: 2,
                  color:"#F7464A",
                  highlight: "#FF5A5E",
                  label: "Windows 10"
              },
              {
                  value: 500,
                  color: "#46BFBD",
                  highlight: "#5AD3D1",
                  label: "Android"
              },
              {
                  value: 50,
                  color: "#CCAABB",
                  highlight: "#CCBBBB",
                  label: "Mac OS X"
              },
              {
                  value: 20,
                  color: "#FFBF70",
                  highlight: "#FFC870",
                  label: "Linux"
              },
              {
                  value: 50,
                  color: "#112288",
                  highlight: "#1122BB",
                  label: "BlackBerry"
              }
          ]
        </script>
      </div>
      <div class="canvas-holder-half">
        <canvas id="browser" height="40" width="50" style="padding-right: 30px;"></canvas>
        <div id="browserLegend"></div>
        <script>
          var browserData = [
              {
                  value: 377,
                  color:"#F7464A",
                  highlight: "#FF5A5E",
                  label: "Firefox"
              },
              {
                  value: 503,
                  color: "#652770",
                  highlight: "#82478C",
                  label: "Google Chrome"
              },
              {
                  value: 323,
                  color: "#4E9231",
                  highlight: "#76B75B",
                  label: "Opera"
              },
              {
                  value: 22,
                  color: "#5864D9",
                  highlight: "#7B85E6",
                  label: "Maxthon"
              },
              {
                  value: 97,
                  color: "#FDB45C",
                  highlight: "#FFC870",
                  label: "Internet Explorer"
              }
          ]
        </script>
      </div>
      <script>
        window.onload = function(){
            var ctx_line = document.getElementById("monthly").getContext("2d");
            window.myLine = new Chart(ctx_line).Line(monthlyData, {
                responsive: true,
                bezierCurve: false,
                pointDot: true,
                datasetFill: true,
            });

            var optionsBrowser = {
              responsive: true,
              legendTemplate: "<ul class=\"doughnut-legend\">"
              + "<% for (var i=0; i<browserData.length; i++){%><li><span style=\"background-color:<%=browserData[i].color%>\">"
              + "</span><%if(browserData[i].label){%><%=browserData[i].label%><%}%></li><%}%></ul>"
            }

            var optionsPlatform = {
              responsive: true,
              legendTemplate: "<ul class=\"doughnut-legend\">"
              + "<% for (var i=0; i<platformData.length; i++){%><li><span style=\"background-color:<%=platformData[i].color%>\">"
              + "</span><%if(platformData[i].label){%><%=platformData[i].label%><%}%></li><%}%></ul>"
            }


            var ctx_pie_i = document.getElementById("platform").getContext("2d");
            var platformChart = new Chart(ctx_pie_i).Pie(platformData, optionsPlatform);
            
            

            var ctx_pie_ii = document.getElementById("browser").getContext("2d");
            var browserChart = new Chart(ctx_pie_ii).Pie(browserData, optionsBrowser);

            document.getElementById("platformLegend").innerHTML = platformChart.generateLegend();
            document.getElementById("browserLegend").innerHTML = browserChart.generateLegend();
        }
      </script>
            <style>
      .doughnut-legend {
        list-style: none outside none;
      }
      .doughnut-legend li {
        display: block;
        position: relative;
        margin-bottom: 4px;
        border-radius: 5px;
        padding: 2px 8px 2px 28px;
        font-size: 14px;
        cursor: default;
        transition: background-color 200ms ease-in-out 0s;
      }
      .doughnut-legend li span {
          display: block;
          position: absolute;
          left: 0px;
          top: 0px;
          width: 20px;
          height: 100%;
          border-radius: 5px;
      }
      </style>
    </div>
  </div>
  <h2>Refering sites & Popular content</h2>
  <div class="panel panel-default">
    <div class="panel-body">
      <h3>Refering sites</h3>
      <table class="table table-bordered">
        <tr>
          <td><strong>Site</strong></td>
          <td><strong>Views</strong></td>
            <td><strong>Unique visitors</strong></td>
        </tr>
        <tr>
          <td>google.cz</td>
          <td>501</td>
          <td>407</td>
        </tr>
        <tr>
          <td>idnes.cz</td>
          <td>452</td>
          <td>370</td>
        </tr>
        <tr>
          <td>vutbr.cz</td>
          <td>327</td>
          <td>224</td>
        </tr>
        <tr>
          <td>seznam.cz</td>
          <td>42</td>
          <td>39</td>
        </tr>
      </table>
      <h3>Popular content</h3>
      <table class="table table-bordered">
        <tr>
          <td>Content</td>
          <td>Views</td>
          <td>Unique visitors</td>
        </tr>
        <tr>
          <td>school-stuff.html</td>
          <td>559</td>
          <td>470</td>
        </tr>
        <tr>
          <td>index.html</td>
          <td>442</td>
          <td>321</td>
        </tr>
        <tr>
          <td>idontknow.html</td>
          <td>321</td>
          <td>249</td>
        </tr>
      </table>
    </div>
  </div>
</div>
