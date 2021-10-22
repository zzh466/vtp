import axios from 'axios';

axios.defaults.withCredentials=true
export default function request(config){
    const url = `http://139.196.41.155:8080/vtpmanagerapi/${config.url}`;
    return axios({...config, url}).then(res => res.data)
} 