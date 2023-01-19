const express = require("express")
const axios = require("axios")
const app = express()

const DB_URL = "http://localhost:3000/data"

app.get("/data/:param", async (req, res) => {
    const param = req.params.param
    console.log("param is", param)
    const data = (await axios.get(DB_URL))["data"]
    console.log(data)
    const paramData = data[param]
    res.send(paramData.toString())
})

const PORT = 37279
app.listen(PORT, () => {
    console.log(`App listening on ${PORT}`)
})