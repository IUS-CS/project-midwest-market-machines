function About() {
  return (
    <div className="about">
      <h1>About Simple Trade</h1>

      <p>
        Simple Trade is an AI-assisted trading application for major cryptocurrencies 
        and stocks. The student authors of this website are Hunter Poole, Duwayne Gray, Ian Olsen, 
        Xavier Olsen, and Tanner Philips. This website was developed as part of a semester-long 
        student project aimed at demonstrating industry software engineering practices, as well as 
        collaborative development practices commonly used in real-world software engineering.
      </p>

      <p>
        The application is designed to run entirely on-device. By doing so, Simple Trade reduces 
        reliance on cloud hosting and inference, helping lower operational costs while also improving 
        privacy and security. Because user data does not need to be processed or stored by the 
        developers, the platform gives users greater control over their information while making 
        better use of the hardware they already own.
      </p>

      <p>
        It is our goal that Simple Trade will truly enable retail traders to maximize their return on 
        investments without becoming a time sink, or worse, a Tool Shaped Object. Rather, Simple Trade 
        will provide immediate value and ease-of-use to retail traders. We strive not to create an application 
        where the product of the application is the experience of setting up and using the application, 
        but rather one that is genuinely useful to its users. 
      </p>

      <p>This project used the following technologies and tools: </p>
      <ul>
        <li>React Native for cross-platform and mobile development (with Windows and macOS out-of-tree support)</li>
        <li>C++ for the backend.</li>
        <li>JavaScript, HTML, and CSS for the frontend.</li>
        <li>CMake as the build system for the C++ code during the prototyping phase.</li>
        <li>Same for Node.js for the JavaScript front end.</li>
        <li>Both to be replaced by React Native’s build system as necessary.</li>
      </ul>

      <p>
        For any more questions or concerns about Simple Trade, feel free to visit
        our GitHub Repository!
      </p>

      <p>
        <a
          href="https://github.com/IUS-CS/project-midwest-market-machines"
          target="_blank"
          rel="noopener noreferrer"
        >
          Visit our GitHub Repository
        </a>
      </p>
    </div>
  );
}

export default About;
