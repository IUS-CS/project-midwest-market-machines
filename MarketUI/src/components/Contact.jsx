import { useState } from 'react'
import emailjs from '@emailjs/browser'

function Contact() {
  const [name, setName] = useState("");
  const [email, setEmail] = useState("");
  const [message, setMessage] = useState("");

  const handleSubmit = (e) => {
    e.preventDefault()

    const templateParams = {
      name,
      email,
      message,
    }

    emailjs.send(
      'service_4y1d1l9',
      'template_doqpwxw',
      templateParams,
      '2ECO1roAi3LSZLdVg'
    )
    .then(() => {
      alert("Your message has been submitted!")
      setName("")
      setEmail("")
      setMessage("")
    })
    .catch((error) => {
      console.error(error)
      alert("Failed to send message.")
    })
  }

  return (
    <div className="contact">
      <h1>Contact Us</h1>

      <form className="contact-form-box" onSubmit={handleSubmit}>
        <input
          type="text"
          placeholder="Your Name"
          value={name}
          onChange={(e) => setName(e.target.value)}
          required
        />

        <input
          type="email"
          placeholder="Your Email"
          value={email}
          onChange={(e) => setEmail(e.target.value)}
          required
        />

        <textarea
          placeholder="Your Message"
          rows="5"
          value={message}
          onChange={(e) => setMessage(e.target.value)}
          required
        ></textarea>

        <button type="submit">Submit</button>
      </form>
    </div>
  );
}

export default Contact;